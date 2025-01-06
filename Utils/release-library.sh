#!/bin/bash

cd ..

REPO="fabcolonna/polito-cas-landtiger-lib"
ASSETS=peripherals-lib.tar.gz

if ! command -v gh &> /dev/null; then
    echo "Error: GitHub CLI (gh) is required, but not installed."
    exit 1
fi

echo "Authenticating with GitHub CLI..."
gh auth status &> /dev/null || gh auth login

# Variables
echo "Creating a new release..."
read -p "Version: " TAG
read -p "Title: " TITLE
read -p "Description: " DESCRIPTION

echo -e "\n=== Summary ===\n"
echo "Repository: $REPO"
echo "Version: $TAG"
echo "Title: $TITLE"
echo "Description: $DESCRIPTION"
echo -e "Assets: ${ASSETS[@]}\n\n"

read -p "Proceed? (y/n): " CONFIRM
if [[ "$CONFIRM" != "y" ]]; then
    echo "Release process aborted."
    exit 1
fi

echo "Checking repository permissions..."
if ! gh repo view "$REPO" &> /dev/null; then
    echo "Error: You don't have access to the repository $REPO."
    exit 1
fi

git tag -a "$TAG" -m "$TITLE"
git push origin "$TAG"
gh release create "$TAG" "${ASSETS[@]}" --title "$TITLE" --notes "$DESCRIPTION"
echo "Release $TAG created successfully!"

rm -f "${ASSETS[@]}"