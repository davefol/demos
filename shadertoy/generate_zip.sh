(SOURCE_DIR=$(basename $PWD) ZIP=cookiecutter.zip && # Set variables
pushd .. && # Set parent directory as working directory
zip -r $ZIP $SOURCE_DIR --exclude $SOURCE_DIR/$ZIP --quiet && # ZIP cookiecutter
mv $ZIP $SOURCE_DIR/$ZIP && # Move ZIP to original directory
popd && # Restore original work directory
echo  "Cookiecutter full path: $PWD/$ZIP")