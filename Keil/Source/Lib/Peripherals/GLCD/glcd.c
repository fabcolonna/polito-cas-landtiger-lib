#include "glcd.h"
#include "cl_list.h"
#include "glcd_config.h"

#include "glcd_lowlevel.h"
#include "glcd_processor.h"

#include <LPC17xx.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// VARIABLES

/// @brief Whether the LCD has been initialized.
_PRIVATE bool s_initialized = false;

/// @brief The memory allocator used to allocate the objects.
_PRIVATE MEM_Allocator *s_allocator;

/// @brief Current LCD maximum X and Y coordinates.
_USED_EXTERNALLY u16 LCDMaxX, LCDMaxY;

/// @brief Current background color of the screen.
_USED_EXTERNALLY LCD_Color LCDCurrentBGColor = LCD_COL_BLACK;

/// @brief The list of fonts that have been loaded into the LCD.
_USED_EXTERNALLY LCD_Font LCDFontList[GLCD_MAX_FONTS] = {0};
_USED_EXTERNALLY u8 LCDFontListSize = 0;

// RENDER LIST

struct __RLItem
{
    LCD_ObjID id;
    LCD_Obj *obj;
    u8 metadata;
};

#define VISIBLE_MASK (0x1)
#define RENDERED_MASK (0x2)

#define RLITEM_SET_VISIBLE(obj) ((obj)->metadata |= VISIBLE_MASK)
#define RLITEM_SET_RENDERED(obj) ((obj)->metadata |= RENDERED_MASK)

#define RLITEM_UNSET_VISIBLE(obj) ((obj)->metadata &= ~VISIBLE_MASK)
#define RLITEM_UNSET_RENDERED(obj) ((obj)->metadata &= ~RENDERED_MASK)

#define RLITEM_IS_VISIBLE(obj) ((obj)->metadata & VISIBLE_MASK)
#define RLITEM_IS_RENDERED(obj) ((obj)->metadata & RENDERED_MASK)

/// @brief Render list, containing all the components to be rendered & metadata.
_PRIVATE CL_List *s_render_list;
_PRIVATE u32 s_render_list_id_ctr = 0;

// PRIVATE FUNCTIONS

_PRIVATE inline bool bboxes_intersect(const LCD_BBox *const a, const LCD_BBox *const b)
{
    // Check for overlap along the x-axis: left edge of a is to the left of the right edge of b
    // AND the right edge of a is to the right of the left edge of b
    return (a->top_left.x < b->bottom_right.x) && (a->bottom_right.x > b->top_left.x) &&
           // Check for overlap along the y-axis: top edge of a is above the bottom edge of b
           // AND the bottom edge of a is below the top edge of b
           (a->top_left.y < b->bottom_right.y) && (a->bottom_right.y > b->top_left.y);
}

_PRIVATE bool alloc_component_object(const LCD_Component *const src, LCD_Component *dest)
{
    dest->pos = src->pos;
    dest->type = src->type;
    dest->cached_bbox = src->cached_bbox;

    switch (src->type)
    {
    case LCD_COMP_LINE:
        if ((dest->object.line = MEM_Alloc(s_allocator, sizeof(LCD_Line))) == NULL)
            return false;

        memcpy(dest->object.line, src->object.line, sizeof(LCD_Line));
        return true;
    case LCD_COMP_RECT:
        if ((dest->object.rect = MEM_Alloc(s_allocator, sizeof(LCD_Rect))) == NULL)
            return false;

        memcpy(dest->object.rect, src->object.rect, sizeof(LCD_Rect));
        return true;
    case LCD_COMP_CIRCLE:
        if ((dest->object.circle = MEM_Alloc(s_allocator, sizeof(LCD_Circle))) == NULL)
            return false;

        memcpy(dest->object.circle, src->object.circle, sizeof(LCD_Circle));
        return true;
    case LCD_COMP_IMAGE:
        if ((dest->object.image = MEM_Alloc(s_allocator, sizeof(LCD_Image))) == NULL)
            return false;

        memcpy(dest->object.image, src->object.image, sizeof(LCD_Image));
        return true;
    case LCD_COMP_TEXT:
        if ((dest->object.text = MEM_Alloc(s_allocator, sizeof(LCD_Text))) == NULL)
            return false;

        memcpy(dest->object.text, src->object.text, sizeof(LCD_Text));
        return true;
    case LCD_COMP_BUTTON:
        if ((dest->object.button = MEM_Alloc(s_allocator, sizeof(LCD_Button))) == NULL)
            return false;

        memcpy(dest->object.button, src->object.button, sizeof(LCD_Button));
        return true;
    }

    return false; // Should never reach this
}

_PRIVATE void dealloc_component_object(LCD_Component *const comp)
{
    switch (comp->type)
    {
    case LCD_COMP_LINE:
        MEM_Free(s_allocator, comp->object.line);
        break;
    case LCD_COMP_RECT:
        MEM_Free(s_allocator, comp->object.rect);
        break;
    case LCD_COMP_CIRCLE:
        MEM_Free(s_allocator, comp->object.circle);
        break;
    case LCD_COMP_IMAGE:
        MEM_Free(s_allocator, comp->object.image);
        break;
    case LCD_COMP_TEXT:
        MEM_Free(s_allocator, comp->object.text);
        break;
    case LCD_COMP_BUTTON:
        MEM_Free(s_allocator, comp->object.button);
        break;
    }
}

// RENDERING FUNCTIONS

// This function needs to control the rendering phase. It will skip object which have
// the rendered property set to true (already rendered). It WON'T TOUCH the visible property,
// it will only update rendered (false->true).
_PRIVATE inline LCD_Error render(struct __RLItem *const item)
{
    // If the object is already rendered, skip it.
    if (RLITEM_IS_RENDERED(item))
        return LCD_ERR_OK;

    if (__LCD_PROC_DoProcessObject(item->obj, &item->obj->bbox, MD_DRAW_BBOX))
    {
        RLITEM_SET_RENDERED(item);
        return LCD_ERR_OK;
    }

    return LCD_ERR_DURING_RENDER;
}

// Just like render(), this function controls the un-rendering phase. It will skip
// items only if they have the rendered property set to false (already un-rendered). It WON'T
// TOUCH the visible property. It will update rendered (true->false).
_PRIVATE LCD_Error unrender(struct __RLItem *const item, bool redraw_underneath)
{
    // If the object is not rendered, skip it
    if (!RLITEM_IS_RENDERED(item))
        return LCD_ERR_OK;

    // If we don't need to redraw the objects underneath, we can apply a rougher method for deleting,
    // which is faster. It's up to the caller to decide whether to redraw the objects underneath or not.
    if (__LCD_PROC_DoProcessObject(item->obj, &item->obj->bbox,
                                   redraw_underneath ? MD_DELETE : (MD_DELETE | MD_DELETE_FAST)))
        RLITEM_UNSET_RENDERED(item);

    if (!redraw_underneath)
        return RLITEM_IS_RENDERED(item) ? LCD_ERR_DURING_UNRENDER : LCD_ERR_OK;

    // If we need to redraw the objects underneath, we need to find out which objects
    // are underneath the object we just un-rendered, and mark them as un-rendered too.
    const LCD_BBox *const bbox = &item->obj->bbox;
    CL_LIST_FOREACH_PTR(struct __RLItem, itm, s_render_list, {
        // Skipping the object we just un-rendered, or invisible ones.
        if (itm->id == item->id || !RLITEM_IS_VISIBLE(itm))
            continue;

        if (bboxes_intersect(bbox, &itm->obj->bbox))
            RLITEM_UNSET_RENDERED(itm);
    });

    return LCD_RMRender();
}

// PUBLIC FUNCTIONS

#include "font_msgothic.h"
#include "font_system.h"

LCD_Error LCD_Init(LCD_Orientation orientation, MEM_Allocator *const alloc, const LCD_Color *const clear_to)
{
    __LCD_LL_Init(orientation);

    // Loading the two default fonts: MS Gothic & System
    LCDFontList[0] = Font_MSGothic;
    LCDFontList[1] = Font_System;
    LCDFontListSize = 2;

    // Allocating the render list
    s_render_list = CL_ListAlloc(alloc, sizeof(struct __RLItem));
    if (!s_render_list)
        return LCD_ERR_NO_MEMORY;

    s_allocator = alloc;
    s_initialized = true;

    if (clear_to)
        LCD_SetBackgroundColor(*clear_to, false);

    return LCD_ERR_OK;
}

// clang-format off

bool LCD_IsInitialized(void) { return s_initialized; }

u16 LCD_GetWidth(void) { return s_initialized ? LCDMaxX : 0; }

u16 LCD_GetHeight(void) { return s_initialized ? LCDMaxY : 0; }

LCD_Coordinate LCD_GetSize(void) { return s_initialized ? (LCD_Coordinate){LCDMaxX, LCDMaxY} : (LCD_Coordinate){0}; }

LCD_Coordinate LCD_GetCenter(void) { return s_initialized ? (LCD_Coordinate){LCDMaxX / 2, LCDMaxY / 2} : (LCD_Coordinate){0}; }

// clang-format on

LCD_Color LCD_GetPointColor(LCD_Coordinate point)
{
    if (point.x >= LCDMaxX || point.y >= LCDMaxY || !s_initialized)
        return LCD_COL_NONE;

    return __LCD_LL_GetPointColor(point.x, point.y);
}

LCD_Error LCD_SetPointColor(LCD_Color color, LCD_Coordinate point)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (color == LCD_COL_NONE)
        return LCD_ERR_OK;

    // Checking if color is already 565
    if ((color & ~(0xFFFF)) != 0)
        color = RGB8_TO_RGB565(color);

    if (point.x >= LCDMaxX || point.y >= LCDMaxY)
        return LCD_ERR_COORDS_OUT_OF_BOUNDS;

    __LCD_LL_SetPointColor(color, point.x, point.y);
    return LCD_ERR_OK;
}

LCD_Error LCD_SetBackgroundColor(LCD_Color color, bool redraw_objects)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (color == LCD_COL_NONE)
        return LCD_ERR_OK;

    if ((color & ~(0xFFFF)) != 0)
        color = RGB8_TO_RGB565(color);

    LCDCurrentBGColor = color;
    __LCD_LL_FillScreen(color);

    // No object is displayed anymore, so we need to change the rendered
    // property of each object in the render queue to false, so that after
    // calling LCD_RMRender(), they will be re-rendered with the new background.
    CL_LIST_FOREACH_PTR(struct __RLItem, itm, s_render_list, { RLITEM_UNSET_RENDERED(itm); });

    // If we don't need to redraw the objects, we can return now.
    if (!redraw_objects)
        return LCD_ERR_OK;

    return LCD_RMRender();
}

// RENDERING

LCD_Error LCD_RMAdd(LCD_Obj *const obj, LCD_ObjID *out_id, u8 options)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (!obj)
        return LCD_ERR_NULL_PARAMS;

    if (!obj->comps || obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    if (obj->comps_size > GLCD_MAX_COMPS_PER_OBJECT)
        return LCD_ERR_TOO_MANY_COMPS_IN_OBJ;

    // Allocating an object:
    LCD_Obj *new_obj = MEM_Alloc(s_allocator, sizeof(LCD_Obj));
    if (!new_obj)
    {
        *out_id = -1;
        return LCD_ERR_NO_MEMORY;
    }

    // Allocating the components of the object
    new_obj->comps = MEM_Alloc(s_allocator, obj->comps_size * sizeof(LCD_Component));
    if (!new_obj->comps)
    {
        *out_id = -1;
        MEM_Free(s_allocator, new_obj);
        return LCD_ERR_NO_MEMORY;
    }

    // Copying the object
    new_obj->comps_size = obj->comps_size;
    new_obj->bbox = (LCD_BBox){0}; // BBox will be calculated by the processor

    // Each component has an object, which is a union of all the possible components. Since the
    // union contains pointers, we need to allocate them separately, based on the type attribute.
    for (u8 i = 0; i < obj->comps_size; i++)
    {
        if (!alloc_component_object(&obj->comps[i], &new_obj->comps[i]))
        {
            *out_id = -1;
            MEM_Free(s_allocator, new_obj->comps);
            MEM_Free(s_allocator, new_obj);
            return LCD_ERR_NO_MEMORY;
        }
    }

    // Adding the object to the render list
    const struct __RLItem item = {
        .id = s_render_list_id_ctr++,
        .obj = new_obj,
        .metadata = (options & LCD_ADD_OBJ_OPT_DONT_MARK_VISIBLE) ? 0 : VISIBLE_MASK,
    };

    if (CL_ListPushBack(s_render_list, &item) != CL_ERR_OK)
    {
        *out_id = -1;
        for (u8 i = 0; i < new_obj->comps_size; i++)
            dealloc_component_object(&new_obj->comps[i]);

        MEM_Free(s_allocator, new_obj->comps);
        MEM_Free(s_allocator, new_obj);
        return LCD_ERR_NO_MEMORY;
    }

    *out_id = item.id;
    return LCD_ERR_OK;
}

LCD_Error LCD_RMRemove(LCD_ObjID id, bool redraw_underneath)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return LCD_ERR_INVALID_OBJ;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item)
        return LCD_ERR_INVALID_OBJ;

    // Additional checks
    if (!item->obj || !item->obj->comps || item->obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    // Un-rendering the object
    LCD_Error err;
    if ((err = unrender(item, redraw_underneath)) != LCD_ERR_OK)
        return err;

    // Deallocating the object
    for (u8 i = 0; i < item->obj->comps_size; i++)
        dealloc_component_object(&item->obj->comps[i]);

    MEM_Free(s_allocator, item->obj->comps);
    MEM_Free(s_allocator, item->obj);
    CL_ListRemoveAt(s_render_list, id, NULL);

    s_render_list_id_ctr--;
    return LCD_ERR_OK;
}

LCD_Error LCD_RMClear(void)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (CL_ListIsEmpty(s_render_list))
        return LCD_ERR_OK;

    // Clearing the render list
    CL_LIST_FOREACH_PTR(struct __RLItem, item, s_render_list, {
        for (u8 i = 0; i < item->obj->comps_size; i++)
            dealloc_component_object(&item->obj->comps[i]);

        MEM_Free(s_allocator, item->obj->comps);
        MEM_Free(s_allocator, item->obj);
    });

    CL_ListClear(s_render_list);

    // Clearing the screen
    __LCD_LL_FillScreen(LCDCurrentBGColor);

    s_render_list_id_ctr = 0;
    return LCD_ERR_OK;
}

LCD_Error LCD_RMRender(void)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    LCD_Error err;
    CL_LIST_FOREACH_PTR(struct __RLItem, itm, s_render_list, {
        if (RLITEM_IS_VISIBLE(itm) && !RLITEM_IS_RENDERED(itm))
        {
            if ((err = render(itm)) != LCD_ERR_OK)
                return err;
        }
    });

    return LCD_ERR_OK;
}

LCD_Error LCD_RMRenderTemporary(LCD_Obj *const obj)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (!obj || !obj->comps || obj->comps_size <= 0)
        return LCD_ERR_INVALID_OBJ;

    return __LCD_PROC_DoProcessObject(obj, NULL, MD_DRAW_BBOX) ? LCD_ERR_OK : LCD_ERR_DURING_RENDER;
}

LCD_Error LCD_RMSetVisibility(LCD_ObjID id, bool visible, bool redraw_underneath)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return LCD_ERR_INVALID_OBJ;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item)
        return LCD_ERR_INVALID_OBJ;

    // Additional checks
    if (!item->obj || !item->obj->comps || item->obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    // If item is already visible, and we want to make it visible, we can skip the operation.
    if (visible && RLITEM_IS_VISIBLE(item))
        return LCD_ERR_OK;

    // If item is not visible, and we want to make it invisible, we can skip the operation.
    if (!visible && !RLITEM_IS_VISIBLE(item))
        return LCD_ERR_OK;

    // If we want to make the object visible, we need to render it.
    if (visible)
    {
        item->metadata |= VISIBLE_MASK;
        if (render(item) == LCD_ERR_OK)
            return LCD_ERR_OK;
        else
        {
            item->metadata &= ~VISIBLE_MASK;
            return LCD_ERR_DURING_RENDER;
        }
    }

    // If we want to make the object invisible, we need to un-render it.
    item->metadata &= ~VISIBLE_MASK;
    if (unrender(item, redraw_underneath) == LCD_ERR_OK)
        return LCD_ERR_OK;

    item->metadata |= VISIBLE_MASK;
    return LCD_ERR_DURING_UNRENDER;
}

bool LCD_RMIsVisible(LCD_ObjID id)
{
    if (!s_initialized || id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return false;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item)
        return false;

    return RLITEM_IS_VISIBLE(item);
}

LCD_Error LCD_RMMove(LCD_ObjID id, LCD_Coordinate new_pos, bool redraw_underneath)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return LCD_ERR_INVALID_OBJ;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item)
        return LCD_ERR_INVALID_OBJ;

    // Additional checks
    if (!item->obj || !item->obj->comps || item->obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    // Starting by de-rendering the previous object
    LCD_Error err;
    if ((err = unrender(item, redraw_underneath)) != LCD_ERR_OK)
        return err;

    // Iterating through each component and update its position
    i16 dx, dy;
    LCD_Component *comp;
    for (u16 i = 0; i < item->obj->comps_size; i++)
    {
        comp = &(item->obj->comps[i]);
        switch (comp->type)
        {
        case LCD_COMP_LINE: {
            // For a line, we need to calculate the BBox, and calculate the offset that
            // needs to be applied to the from & to coordinates with the top-left corner
            // of the BBox, and new_pos, which in this case is interpreted as the new
            // top-left corner of the line.
            LCD_BBox *bbox = &item->obj->bbox;
            dx = new_pos.x - bbox->top_left.x;
            dy = new_pos.y - bbox->top_left.y;

            comp->object.line->from.x += dx;
            comp->object.line->from.y += dy;
            comp->object.line->to.x += dx;
            comp->object.line->to.y += dy;
            break;
        }
        case LCD_COMP_CIRCLE:
            dx = new_pos.x - comp->object.circle->center.x;
            dy = new_pos.y - comp->object.circle->center.y;

            // Calculate the offset
            comp->object.circle->center.x += dx;
            comp->object.circle->center.y += dy;
            break;
        case LCD_COMP_RECT:
        case LCD_COMP_IMAGE:
        case LCD_COMP_TEXT:
        case LCD_COMP_BUTTON:
            dx = new_pos.x - comp->pos.x;
            dy = new_pos.y - comp->pos.y;

            comp->pos.x += dx;
            comp->pos.y += dy;
            break;
        }
    }

    return render(item);
}

// PUBLIC FONT MANAGER FUNCTIONS

LCD_Error LCD_FMAddFont(LCD_Font font, LCD_FontID *out_id)
{
    if (LCDFontListSize >= GLCD_MAX_FONTS)
        return LCD_ERR_FONT_LIST_FULL;

    *out_id = LCDFontListSize;
    LCDFontList[*out_id] = font;
    LCDFontListSize++;
    return LCD_ERR_OK;
}

LCD_Error LCD_FMRemoveFont(LCD_FontID id)
{
    if (id >= LCDFontListSize || id <= 1) // Cannot remove the default fonts
        return LCD_ERR_INVALID_FONT_ID;

    for (u32 i = id; i < LCDFontListSize - 1; i++)
        LCDFontList[i] = LCDFontList[i + 1];

    LCDFontListSize--;
    return LCD_ERR_OK;
}

// BBOX

LCD_Error LCD_GetBBox(LCD_ObjID id, LCD_BBox *out_bbox)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (!out_bbox)
        return LCD_ERR_NULL_PARAMS;

    if (id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return LCD_ERR_INVALID_OBJ;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item || !item->obj)
        return LCD_ERR_INVALID_OBJ;

    *out_bbox = item->obj->bbox;
    return LCD_ERR_INVALID_OBJ;
}

LCD_Error LCD_CalcBBoxForObject(const LCD_Obj *const obj, LCD_BBox *out_bbox)
{
    if (!obj || !out_bbox)
        return LCD_ERR_NULL_PARAMS;

    if (!obj->comps || obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    const bool res = __LCD_PROC_DoProcessObject(obj, out_bbox, MD_BBOX);
    return res ? LCD_ERR_OK : LCD_ERR_DURING_BBOX_CALC;
}

// PUBLIC DEBUG FUNCTIONS

LCD_Error LCD_DEBUG_RenderBBox(LCD_ObjID id)
{
    if (!s_initialized)
        return LCD_ERR_UNINITIALIZED;

    if (id < 0 || (u32)id >= CL_ListSize(s_render_list))
        return LCD_ERR_INVALID_OBJ;

    struct __RLItem *item = NULL;
    if (CL_ListGetPtr(s_render_list, id, (void **)&item) != CL_ERR_OK || !item)
        return LCD_ERR_INVALID_OBJ;

    if (!item->obj || !item->obj->comps || item->obj->comps_size == 0)
        return LCD_ERR_INVALID_OBJ;

    const LCD_Dimension dim = {
        .width = abs(item->obj->bbox.bottom_right.x - item->obj->bbox.top_left.x),
        .height = abs(item->obj->bbox.bottom_right.y - item->obj->bbox.top_left.y),
    };

    // A bbox is always a rectangle, so we can render it as a rectangle directly.
    // clang-format off
    const LCD_Obj obj = LCD_OBJECT_DEFINE(
        LCD_RECT(item->obj->bbox.top_left, {
            .width = dim.width, .height = dim.height,
            .edge_color = LCD_COL_RED, .fill_color = LCD_COL_NONE,
        }),
        LCD_LINE({
            .from = item->obj->bbox.top_left,
            .to = item->obj->bbox.bottom_right,
            .color = LCD_COL_GREEN,
        }),
        LCD_LINE({
            .from = (LCD_Coordinate){
                .x = item->obj->bbox.top_left.x + dim.width, 
                .y = item->obj->bbox.top_left.y,
            },
            .to = (LCD_Coordinate){
                .x = item->obj->bbox.top_left.x, 
                .y = item->obj->bbox.top_left.y + dim.height
            },
            .color = LCD_COL_BLUE,
        })
    );

    const bool res = __LCD_PROC_DoProcessObject(&obj, NULL, MD_DRAW_BBOX);
    // clang-format on

    return res ? LCD_ERR_OK : LCD_ERR_DURING_RENDER;
}