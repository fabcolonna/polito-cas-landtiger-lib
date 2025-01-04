#!/bin/bash

[[ -z $(git status -s) ]] || { 
    echo "Working directory is not clean. Please commit or stash your changes first."; 
    exit 1;
}

TEMP_DOCS=$(mktemp -d)
echo "Generating Doxygen docs in ${TEMP_DOCS}..."
doxygen ../Doxyfile
mv docs/html/* "$TEMP_DOCS"
rm -rf docs/

echo "Moving html/ to gh-pages branch..."
git checkout gh-pages
rm -rf *
cp -r "$TEMP_DOCS"/* .

echo "Committing on gh-pages branch..."
git add .
git commit -m "Update Doxygen documentation"
git push origin gh-pages

echo "Switching back to main branch..."
git checkout main
rm -rf "$TEMP_DOCS"

echo "Done!"