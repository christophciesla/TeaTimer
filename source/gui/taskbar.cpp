#include "gui/taskbar.h"

#include <QtWidgets>

#ifdef WIN32
#include <Shobjidl.h>
#endif

#include <iostream>

namespace gui
{
    class TaskbarImpl
    {
    public:
        TaskbarImpl(QWidget* window)
#ifdef WIN32
            : window_handle_{0}
            , taskbar_item_{nullptr}
#endif
        {
#ifdef WIN32
            if (window)
            {
                window_handle_ = reinterpret_cast<HWND>(window->winId());
            }
            std::ignore = CoCreateInstance(CLSID_TaskbarList, 0, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, reinterpret_cast< void** >(&taskbar_item_));
#endif
        }

        ~TaskbarImpl()
        {
#ifdef WIN32
            if (taskbar_item_)
            {
                taskbar_item_->Release();
                taskbar_item_ = nullptr;
            }
            window_handle_ = 0;
#endif
        }

        void Update(const bool progress_enabled, const std::int32_t current, const std::int32_t total)
        {
#ifdef WIN32
            if (taskbar_item_ && window_handle_)
            {
                if (progress_enabled)
                {
                    taskbar_item_->SetProgressValue(window_handle_, current, total);
                }
                else
                {
                    taskbar_item_->SetProgressState(window_handle_, TBPF_NOPROGRESS);
                }
            }
#else
            std::ignore = progress_enabled;
            std::ignore = current;
            std::ignore = total;
#endif
        }

    private:
#ifdef WIN32
        HWND window_handle_;
        ITaskbarList3* taskbar_item_;
#endif
    };

    Taskbar::Taskbar(QWidget* window):
        QObject(window),
        impl_(new TaskbarImpl(window))
    {
    }

    Taskbar::~Taskbar()
    {
        delete impl_;
        impl_ = nullptr;
    }

    void Taskbar::Update(const bool progress_enabled, const std::int32_t current, const std::int32_t total)
    {
        impl_->Update(progress_enabled, current, total);
    }
}
