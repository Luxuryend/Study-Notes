#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QStandardItemModel>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    // 【核心代码】声明事件过滤器虚函数
    // 参数1：obj 是发送事件的对象（比如是按钮还是滑块）
    // 参数2：event 是发生的具体事件（比如按下、松开、移动）
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_pushButton_play_clicked();

    void on_verticalSlider_valueChanged(int value);

    void playAudioByRow(int row);

    void on_pushButton_prev_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_volume_clicked();

private:
    Ui::Widget *ui;

    bool is_play;
    QStandardItemModel *m_model;

    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;

    QString formatTime(qint64 ms) {
        qint64 totalSeconds = ms / 1000;
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        // 使用 arg 填充零，保证显示为 02:05 而不是 2:5
        return QString("%1:%2")
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
};
#endif // WIDGET_H
