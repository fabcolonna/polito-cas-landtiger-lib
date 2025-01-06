#!/bin/bash

cd ..

[[ -z $(git status -s) ]] || { 
    echo "Working directory is not clean. Please commit or stash your changes first."; 
    exit 1;
}

TEMP_DOCS=$(mktemp -d)
echo "Generating Doxygen docs in ${TEMP_DOCS}..."
doxygen Doxyfile >/dev/null 2>&1
mv Docs/html/* "$TEMP_DOCS"
rm -rf Docs/

echo "Moving html/ to gh-pages branch..."
git checkout gh-pages >/dev/null 2>&1
rm -rf *
mv "$TEMP_DOCS"/* .
rm -rf "$TEMP_DOCS"

echo "Committing on gh-pages branch..."
git add . >/dev/null 2>&1
git commit -m "Update Doxygen documentation" >/dev/null 2>&1
git push origin gh-pages >/dev/null 2>&1

echo "Switching back to main branch..."
git checkout main >/dev/null 2>&1
echo "Done!"