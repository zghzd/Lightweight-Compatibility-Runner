# 添加新平台支持时
1.在`lcr`项目中新建对应的`platform-*.h`，
其中`*`为平台名称，首字母大写。  
2.在`lcr.h`中include对应的`platform-*.h`。  
3.在`lcr.cpp`的对应`if`块(平台if块和文件类型if块)中，完成功能调用。
