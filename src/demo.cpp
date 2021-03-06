﻿#pragma warning(disable:4996)
#include "cqsdk/cqsdk.h"
#include <Windows.h>
#include <thread>
#include "MagicConch/MagicConch.h"

namespace app = cq::app; // 插件本身的生命周期事件和管理
namespace event = cq::event; // 用于注册 QQ 相关的事件处理函数
namespace api = cq::api; // 用于调用酷 Q 提供的接口
namespace logging = cq::logging; // 用于日志
namespace message = cq::message; // 提供封装了的 Message 等类

MagicConch myConch;	//创建我们的海螺

// 初始化 App Id
CQ_INITIALIZE("com.company.MagicConch");

// 插件入口，在静态成员初始化之后，app::on_initialize 事件发生之前被执行，用于配置 SDK 和注册事件回调
CQ_MAIN {
    cq::config.convert_unicode_emoji = true; // 配置 SDK 自动转换 Emoji 到 Unicode（默认就是 true）

    app::on_enable = [] {
        // logging、api、dir 等命名空间下的函数只能在事件回调函数内部调用，而不能直接在 CQ_MAIN 中调用
        logging::debug(u8"启用", u8"插件已启动");
		
		myConch.multiTasks();
    };

    event::on_private_msg = [](const cq::PrivateMessageEvent &e) {
        logging::debug(u8"消息", u8"收到私聊消息：" + e.message + u8"，发送者：" + std::to_string(e.user_id));

        try {
			myConch.InterfaceOfPrivateMsg(e);
            
        } catch (const cq::exception::ApiError &err) {
            // API 调用失败
            logging::debug(u8"API", u8"调用失败，错误码：" + std::to_string(err.code));
        }

        e.block(); // 阻止事件继续传递给其它插件
    };

    event::on_group_msg = [](const cq::GroupMessageEvent &e) {
        try {

			myConch.InterfaceOfGroupMsg(e);
			
        } catch (const cq::exception::ApiError &err) {
            cq::logging::warning(
                u8"MagicConch",
                u8"发生std expetion: " + std::to_string(err.what()) + u8"\n发送消息为: " + e.raw_message);
        } catch (...) {
            logging::warning(u8"MagicConch", u8"发生无法捕捉的异常，发送消息为: " + e.raw_message);
        }
        e.block();	//阻止其他插件获取消息
    };
}