@echo ********正在合成BIN文件********
SET VERSION=%1
bootgen -image build/release/acur101_app_system.bif -o build/release/%VERSION%.bin -arch zynqmp -w





