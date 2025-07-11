project(stdhep)

add_compile_options(-Wno-error=incompatible-function-pointer-types)
get_property(mcfio_Include TARGET mcfio PROPERTY INCLUDE_DIRECTORIES)

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(BEFORE ${mcfio_Include})

#message(STATUS "mcfio_Include: ${mcfio_Include}")
#
#message(STATUS "Include directories: ")
#get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#foreach(dir ${dirs})
#    message(STATUS "dir='${dir}'")
#endforeach()

add_library(${PROJECT_NAME}
        stdhep/adumtmp.F
        stdhep/cnv1998to2000.F
        stdhep/cnv2000to2002.F
        stdhep/cnv2000to2002lst.F
        stdhep/cnv2000to2004.F
        stdhep/cnv2000to2004lst.F
        stdhep/cnv2004to2006.F
        stdhep/cnv2004to2006lst.F
        stdhep/cnv98to00lst.F
        stdhep/dpmhep.F
        stdhep/dpmtran.F
        stdhep/dumphepeup.F
        stdhep/dumpheprup.F
        stdhep/gtran.F
        stdhep/hep2geant.F
        stdhep/hepchg.F
        stdhep/hepchg_2002.F
        stdhep/hepchg_2004.F
        stdhep/hepchg_old.F
        stdhep/hepcmp.F
        stdhep/hepcmp_2002.F
        stdhep/hepcmp_2004.F
        stdhep/hepcmp_old.F
        stdhep/heplst.F
        stdhep/hepnam.F
        stdhep/hepnam_2002.F
        stdhep/hepnam_2004.F
        stdhep/hepnam_old.F
        stdhep/heptree.F
        stdhep/hptrlstdpm.F
        stdhep/hptrlsth.F
        stdhep/hptrlsti.F
        stdhep/hptrlstj.F
        stdhep/hptrlstq.F
        stdhep/hwghep.F
        stdhep/hwtran.F
        stdhep/isahep.F
        stdhep/istran.F
        stdhep/lunhep.F
        stdhep/luntmp.F
        stdhep/lutran.F
        stdhep/parsecsv.F
        stdhep/parsecsv_2004.F
        stdhep/pdgprtb.F
        stdhep/pdgrdcsvtb.F
        stdhep/pdgrdtb.F
        stdhep/pdgtonew.F
        stdhep/pdgtran.F
        stdhep/pythia2ev4.F
        stdhep/qqhep.F
        stdhep/qqtran.F
        stdhep/readmadgraph.F
        stdhep/std3to4.F
        stdhep/std3to4lst.F
        stdhep/std3to4prt.F
        stdhep/std3to4tran.F
        stdhep/stdchg.F
        stdhep/stdchgdsclst.F
        stdhep/stdcomp.F
        stdhep/stdcquarks.F
        stdhep/stddautr.F
        stdhep/stddautrlst.F
        stdhep/stddecayqq.F
        stdhep/stddscndlst.F
        stdhep/stdexpname.F
        stdhep/stdfixmass.F
        stdhep/stdfixpart.F
        stdhep/stdflhwxsec.F
        stdhep/stdflisxsec.F
        stdhep/stdflpyxsec.F
        stdhep/stdlst.F
        stdhep/stdmsjet.F
        stdhep/stdparent.F
        stdhep/stdparentlst.F
        stdhep/stdprthd.F
        stdhep/stdqqadd.F
        stdhep/stdqqdcy.F
        stdhep/stdqquset.F
        stdhep/stdquarkcnt.F
        stdhep/stdquarklst.F
        stdhep/stdquarks.F
        stdhep/stdrotboost.F
        stdhep/stdsort.F
        stdhep/stdspin.F
        stdhep/stdstdsclst.F
        stdhep/stdtcopy.F
        stdhep/stdversn.F
        stdhep/stdvtxmatch.F
        stdhep/stdxend.F
        stdhep/stdxrd.F
        stdhep/stdxrdm.F
        stdhep/stdxrinit.F
        stdhep/stdxropen.F
        stdhep/stdxwcm1.F
        stdhep/stdxwevt.F
        stdhep/stdxwevtlh.F
        stdhep/stdxwevtup.F
        stdhep/stdxwinit.F
        stdhep/stdxwopen.F
        stdhep/stdxwrt.F
        stdhep/stdzero.F
        stdhep/stdzeroqq.F
        stdhep/zerohepeup.F
        stdhep/mcf_StdHep_cxx.c
        stdhep/mcf_Stdhep_xdr.c
        stdhep/mcf_hepev4_xdr.c
        stdhep/mcf_hepup_xdr.c
        stdhep/mcf_stdcm1_xdr.c
        stdhep/copy_stdevent.c
        stdhep/sum_4vec_d.c
        stdhep/sum_4vec_f.c
        )

add_library(${PROJECT_NAME}C
        stdhep/copy_stdevent.c
        stdhep/mcf_StdHep_cxx.c
        stdhep/mcf_Stdhep_xdr.c
        stdhep/mcf_hepev4_xdr.c
        stdhep/mcf_hepup_xdr.c
        stdhep/mcf_stdcm1_xdr.c
        stdhep/stdhep_internal_utils.c
        stdhep/stdhep_mcfio.c
        stdhep/sum_4vec_d.c
        stdhep/sum_4vec_f.c
        )

target_link_libraries(${PROJECT_NAME} PUBLIC mcfio)
install(TARGETS ${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}C)