################################################################################
# Package: AlignmentDBVisualisationTool
################################################################################
gaudi_subdir(AlignmentDBVisualisationTool v1r6)

gaudi_depends_on_subdirs(Gaudi)

find_package(pyanalysis)
find_package(pytools)

gaudi_install_python_modules()
gaudi_install_scripts()


gaudi_add_test(QMTest QMTEST)
