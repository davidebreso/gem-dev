include ../../makedefs.i

CC = wcc
CFLAGS = -bt=dos -ml -s -os -0 -i../../include -i..

OBJECTS = ems_all.obj ems_fp.obj ems_free.obj ems_fs.obj ems_map.obj ems_np.obj ems_rmap.obj \
	  ems_smap.obj ems_vers.obj emslib.obj ob_dofl.obj ob_dost.obj ob_dr_di.obj \
	  ob_g_txt.obj ob_isfl.obj ob_isst.obj ob_s_txt.obj ob_ud_di.obj ob_udfl.obj \
          ob_udst.obj ob_xywh.obj rc_copy.obj rc_equal.obj rc_gr2ar.obj rc_insid.obj \
          rc_inter.obj x_dux_sc.obj x_dux_xf.obj x_duy_sc.obj x_duy_xf.obj x_mul_di.obj \
          x_saspec.obj x_sxform.obj x_udx_sc.obj x_udx_xf.obj x_udx_yf.obj x_udy_sc.obj \
          x_ytox.obj xfmlib.obj formdo.obj ev_event.obj trimage.obj \
	  wccd000.obj wccd001.obj wccd002.obj wccd003.obj wccd004.obj wccd005.obj \
	  wccd006.obj wccd007.obj wccd008.obj wccd009.obj wccd010.obj wccd011.obj \
	  wccd012.obj wccd013.obj wccd014.obj wccd015.obj wccd016.obj wccd017.obj \
	  wccd018.obj wccd019.obj wccd020.obj wccd021.obj wccd022.obj wccd023.obj \
	  wccd024.obj wccd025.obj wccd026.obj wccd027.obj wccd028.obj wccfdraw.obj \
	  wccg000.obj wccg001.obj wccg002.obj wccg003.obj wccg004.obj wccg005.obj \
	  wccg006.obj wccg007.obj wccg008.obj wccg009.obj wccg010.obj wccg011.obj \
	  wccg012.obj wccg013.obj wccg014.obj wccg015.obj wccg016.obj wccg017.obj \
	  wccg018.obj wccg019.obj wccg020.obj wccg021.obj wccg022.obj wccg023.obj \
	  wccg024.obj wccg025.obj wccg026.obj wccg027.obj wccg028.obj wccg029.obj \
	  wccg030.obj wccg031.obj wccg032.obj wccg033.obj wccg034.obj wccg035.obj \
	  wccg036.obj wccg037.obj wccg038.obj wccg039.obj wccg040.obj wccg041.obj \
	  wccg042.obj wccg043.obj wccg044.obj wccg045.obj wccg046.obj wccg047.obj \
	  wccg048.obj wccg049.obj wccg050.obj wccg051.obj wccg052.obj wccg053.obj \
	  wccg054.obj wccg055.obj wccg056.obj wccg057.obj wccg058.obj wccg059.obj \
	  wccg060.obj wccg061.obj wccg062.obj wccg063.obj wccg064.obj wccg065.obj \
	  wccg066.obj wccg067.obj wccg068.obj wccg069.obj wccg070.obj wccg071.obj \
	  wccg072.obj wccg073.obj wccg074.obj wccg075.obj wccg076.obj wccg077.obj \
	  wccg078.obj wccg079.obj wccg080.obj wccg081.obj wccg082.obj wccg083.obj \
	  wccg084.obj wccg085.obj wccg086.obj wccg087.obj wccg088.obj wccg089.obj \
	  wccg090.obj wccg091.obj wccg092.obj wccg093.obj wccg094.obj wccg095.obj \
	  wccg096.obj wccg097.obj \
	  wccgcrt.obj wccgem.obj wcchook.obj \
	  wccv000.obj wccv001.obj wccv002.obj wccv003.obj wccv004.obj \
	  wccv005.obj wccv006.obj wccv007.obj wccv008.obj wccv009.obj wccv010.obj \
	  wccv011.obj wccv012.obj wccv013.obj wccv014.obj wccv015.obj wccv016.obj \
	  wccv017.obj wccv018.obj wccv019.obj wccv020.obj wccv021.obj wccv022.obj \
	  wccv023.obj wccv024.obj wccv025.obj wccv026.obj wccv027.obj wccv028.obj \
	  wccv029.obj wccv030.obj wccv031.obj wccv032.obj wccv033.obj wccv034.obj \
	  wccv035.obj wccv036.obj wccv037.obj wccv038.obj wccv039.obj wccv040.obj \
	  wccv041.obj wccv042.obj wccv043.obj wccv044.obj wccv045.obj wccv046.obj \
	  wccv047.obj wccv048.obj wccv049.obj wccv050.obj wccv051.obj wccv052.obj \
	  wccv053.obj wccv054.obj wccv055.obj wccv056.obj wccv057.obj wccv058.obj \
	  wccv059.obj wccv060.obj wccv061.obj wccv062.obj wccv063.obj wccv064.obj \
	  wccv065.obj wccv066.obj wccv067.obj wccv068.obj wccv069.obj wccv070.obj \
	  wccv071.obj wccv072.obj wccv073.obj wccv074.obj wccv075.obj wccv076.obj \
	  wccv077.obj wccv078.obj wccv079.obj wccv080.obj wccv081.obj wccv082.obj \
	  wccv083.obj wccv084.obj wccv085.obj wccv086.obj wccv087.obj wccv088.obj \
	  wccv089.obj wccv090.obj wccv091.obj wccv092.obj wccv093.obj wccv094.obj \
	  wccv095.obj wccv096.obj wccv097.obj wccv098.obj wccv099.obj wccv100.obj \
	  wccv101.obj wccv102.obj wccv103.obj wccv104.obj wccv105.obj wccv106.obj \
	  wccv107.obj wccv108.obj wccv109.obj wccv110.obj wccv111.obj wccv112.obj \
	  wccv113.obj wccv114.obj wccv115.obj wccv116.obj wccv117.obj wccv118.obj \
	  wccv119.obj wccv120.obj wccv121.obj wccv122.obj wccv123.obj wccv124.obj \
	  wccv125.obj wccv126.obj wccv127.obj wccv128.obj wccv129.obj wccv130.obj \
	  wccv131.obj wccv132.obj wccv133.obj wccv134.obj wccv135.obj wccv136.obj \
          wccv137.obj wccv138.obj wccvdi.obj wcclptr.obj

all:	wccgem-l.lib

wccgem-l.lib:	$(OBJECTS)
	wlib -n wccgem-l.lib $(OBJECTS)

%.obj:	../%.c
	$(CC) -fo=$@ $(CFLAGS) $<

tidy:
	rm -f *.bak *.obj *.err wccgem-l.lib

clean:	tidy
