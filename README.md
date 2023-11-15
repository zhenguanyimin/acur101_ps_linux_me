# acur101_ps_linux

反无雷达嵌入式软件代码，Linux系统

文件结构说明：
sdk: 为petalinux的工程，生成系统包，包含boot, kernel image, xen, system.dtb, rootfs
linux_app: 为业务APP 
rtos_app: 为RTOS端的APP 


CI:
- 版本号格式: ACUR100-Vxx.xx.xx-COMPILE_DATE
    1. ACUR100: 本产品固定代号
    2. Vxx.xx.xx: 默认为V99.99.99，可以通过推送标签指定
    3. COMPILE_DATE: 编译时间，由脚本自动生成
- 发布版本请通过推送标签触发，例如 `git tag V1.00.01 && git push origin V1.00.01`
