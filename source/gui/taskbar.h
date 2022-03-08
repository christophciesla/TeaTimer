#ifndef GUI_TASKBAR_H
#define GUI_TASKBAR_H

#include <QtWidgets>
#include <cstdint>

namespace gui
{
    class TaskbarImpl;

    class Taskbar : public QObject
    {
    public:
        Taskbar(QWidget* window);
        ~Taskbar() override;

        void Update(const bool progress_enabled, const std::int32_t current, const std::int32_t total);

    private:
        TaskbarImpl* impl_;
    };
}

#endif // GUI_TASKBAR_H
