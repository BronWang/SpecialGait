# SpecialGait 
基于Qt的特殊步态调试软件，用于调试Ikid Robot的特殊步态，如前后向跌倒爬起、踢球等。  
Special Gait debug software built by Qt, which is used to debug the special gait of the Ikid Robot, such as forward and backward falling, climbing, kicking, etc.
![image](https://github.com/BronWang/SpecialGait/blob/special_gait_wp/images/software_GUI.png)

# SpecialGait v1.1.0
做了以下更新：
1. 修复了点击表格中相应元素时，其所属帧不能在仿真环境中正常执行的问题
2. 修复了改变步态帧率时，其不会实时起作用的问题
3. 把spinbox改为了不追踪数值变化的模式
4. 优化了软件操作性

# SpecialGait v1.2.0
做了以下更新：
1. 添加了帧率列，关键帧之间的插帧数可调，主界面右边栏的步态帧率设置目前不再起作用
2. 软件增加了步态文件上传功能，接收服务端在IKID_WS项目中
3. 优化了软件的高亮显示
4. 添加了文件上传报错信息输出

# SpecialGait v1.2.1
做了以下更新：
1. 修复了零点调试时与自身数值叠加输出的bug

# 2023/5/10
1. 英文主界面
2. 添加机器人步行调试功能

# 2023/5/11
1. 修复了文件上传bug

# 2023/7/8
改用tcp udp混合模式, udp用来专门负责滑块和连续执行的帧数据传输

# 2023/9/30
1.添加了特殊步态执行功能
2.添加了步态号说明

# 2023/10/4
1.修复文件格式错误，软件闪退bug
2.添加记录当前帧后自动保存功能

# 2023/12/10
1.改进零点调节形式

