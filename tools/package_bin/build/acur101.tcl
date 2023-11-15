set VITIS_WORKSPACE [lindex $argv 0]
set XSA [lindex $argv 1]
set PLATFORM_SPR code/acur101_plt/platform.spr
puts "platform read $PLATFORM_SPR"
platform read $PLATFORM_SPR
puts "platform active"
platform active {acur101_plt}
puts "platform config -updatehw  $XSA"
platform config -updatehw  $XSA
puts "platform generate"
platform generate
puts "setws $VITIS_WORKSPACE"
setws $VITIS_WORKSPACE
puts "app build -name acur101_app"
app build -name acur101_app
puts "build finish"