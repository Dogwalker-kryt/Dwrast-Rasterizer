# Thin wrapper around CMake, so `make clean && make` just works.
# The real build config stays in CMakeLists.txt.
#
#   make                 configure (if needed) + build into build/
#   make clean           delete build/
#   make rebuild         clean + build
#   make gtk             real time preview of MODEL
#   make bench           headless benchmark of MODEL
#   make asan            address/UB sanitizer build into build-asan/
#   make models          generate all test models into models/
#
# variables: MODEL=models/knot_400k.obj FRAMES=200 W=1920 H=1080 ARGS="..."

BUILD_DIR  ?= build
ASAN_DIR   ?= build-asan
MODELS_DIR ?= models
GENERATOR  ?= $(if $(shell command -v ninja 2>/dev/null),Ninja,Unix Makefiles)

BIN        := $(BUILD_DIR)/dwrast_rasterizer
GEN_MODELS := scripts/gen_test_models.py
ALL_MODELS := $(addprefix $(MODELS_DIR)/,torus.obj cube.obj knot_100k.obj knot_400k.obj)

MODEL  ?= $(MODELS_DIR)/knot_100k.obj
FRAMES ?= 200
SIZE_ARGS = $(if $(W),-w $(W)) $(if $(H),-h $(H))

ASAN_FLAGS := -fsanitize=address,undefined -fno-omit-frame-pointer -g

.PHONY: all build clean rebuild gtk bench asan models help

all: build

$(BUILD_DIR)/CMakeCache.txt:
	cmake -S . -B $(BUILD_DIR) -G "$(GENERATOR)"

# cmake --build re-runs the configure step by itself when CMakeLists.txt changes
build: $(BUILD_DIR)/CMakeCache.txt
	cmake --build $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(ASAN_DIR)

rebuild: clean
	$(MAKE) build

# no prerequisite on the script on purpose, so your own models in models/ never get overwritten
$(MODELS_DIR)/%.obj:
	python3 $(GEN_MODELS) $@

models: $(ALL_MODELS)

gtk: build $(MODEL)
	$(BIN) -gtk -obj $(MODEL) -z $(SIZE_ARGS) $(ARGS)

bench: build $(MODEL)
	$(BIN) -obj $(MODEL) -z -bench $(FRAMES) $(SIZE_ARGS) $(ARGS)

$(ASAN_DIR)/CMakeCache.txt:
	cmake -S . -B $(ASAN_DIR) -G "$(GENERATOR)" \
		-DCMAKE_C_FLAGS="$(ASAN_FLAGS)" \
		-DCMAKE_CXX_FLAGS="$(ASAN_FLAGS)" \
		-DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

asan: $(ASAN_DIR)/CMakeCache.txt
	cmake --build $(ASAN_DIR)

help:
	@sed -n '1,13p' Makefile | sed 's/^# \{0,1\}//'
