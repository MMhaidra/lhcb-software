################################################################################
# Package: RichIFBAnalysis
################################################################################
gaudi_subdir(RichIFBAnalysis v1r5)

gaudi_depends_on_subdirs(Rich/RichKernel)

gaudi_add_module(RichIFBAnalysis
                 src/*.cpp
                 LINK_LIBRARIES RichKernelLib)

