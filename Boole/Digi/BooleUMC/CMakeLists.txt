################################################################################
# Package: BooleUMC
################################################################################
gaudi_subdir(BooleUMC v3r0)

gaudi_depends_on_subdirs(Det/FTDet
                         Det/OTDet
                         Det/STDet
                         Det/VPDet
                         Det/VeloDet
                         Event/DigiEvent
                         Event/FTEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiGSL
                         GaudiKernel
                         Kernel/LHCbMath
                         Kernel/MCInterfaces
                         ST/STKernel)

find_package(GSL)

gaudi_add_module(BooleUMC
                 src/*.cpp
                 INCLUDE_DIRS GSL Event/DigiEvent Event/FTEvent Kernel/MCInterfaces
                 LINK_LIBRARIES GSL FTDetLib OTDetLib STDetLib VPDetLib VeloDetLib LinkerEvent MCEvent GaudiAlgLib GaudiGSLLib GaudiKernel LHCbMathLib STKernelLib)

