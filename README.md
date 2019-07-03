![final](https://raw.githubusercontent.com/MregXN/Blogs/master/image/final.jpg)



# PlotClock

用超丑的字体报时。利用STM32F103RC主控，DS1302获取时间，9g舵机驱动手臂，内部跑了freertos系统，具体的制作流程可参考[Blogs](https://github.com/MregXN/Blogs/issues/3)。



## 使用要求

- keil5 打开工程
- JLINK 烧录代码 
- 接线方式： 
    DS1302： RST （PA5）  IO（ PA6）  SCK （PA7） +5V 供电
    舵机： 左臂（PC6） 右臂（PC7） 抬臂（PC8） +5V供电
- 机架安装： 淘宝上找Plotclock机架 30~40应该就买的到



## 目录结构

**PlotClock**

├── CORE                              // 启动文件

├── FreeRTOS                      // FreeRTOS  

├── HARDWARE                   // 外设驱动

├── MALLOC                        // 内存管理文件

├── OBJ                                 // 编译中间文件

├── STM32F10x_FWLib      // 库文件

├── SYSTEM                         //系统配置文件

└── USER                              // main.c  工程入口等



## 版本说明

##### V1.0.0

  初始版本，实现每分钟书写的基本功能。




## 作者

作者：MregXN

邮箱：mregxn@gmail.com




## 鸣谢

该项目参考了项目原作者 [Johannes Heberlein](https://www.thingiverse.com/thing:248009)的[demo](https://github.com/9a/plotclock),以及[正点原子论坛](http://www.openedv.com)关于STM32的例程及实验




