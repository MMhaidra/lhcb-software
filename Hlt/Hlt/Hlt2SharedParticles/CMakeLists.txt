################################################################################
# Package: Hlt2SharedParticles
################################################################################
gaudi_subdir(Hlt2SharedParticles v7r9)

gaudi_depends_on_subdirs(Hlt/HltLine)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
