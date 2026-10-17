prebuild:
	bldmake bldfiles gcce urel

build:
	echo "#define USE_OPENC 1" > SDL_gfx.mmh 
	abld build -v gcce urel

build_estlib:
	echo "" > SDL_gfx.mmh 
	abld build -v gcce urel

clean: 
	cd group && abld reallyclean gcce urel


