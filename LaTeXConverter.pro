QT += widgets
TARGET = LaTeXConverter
SOURCES += main.cpp
# 添加Windows平台专用配置
win32 {
    # 指定生成控制台（调试用）或窗口程序
    CONFIG += c++11
    # 禁用命令行窗口（正式发布时使用）
    # CONFIG -= console
    # 添加应用程序图标（可选）
    # RC_FILE = app.rc
}