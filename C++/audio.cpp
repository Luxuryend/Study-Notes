#include "widget.h"
#include "./ui_widget.h"

#include <QAudioOutput>
#include <QSettings>
#include <QMouseEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 监听整个窗口的事件
    this->installEventFilter(this);
    // 同时也确保滑块和按钮已经初始化
    ui->verticalSlider->hide();

    is_play = false;

    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(4);
    m_model->setHeaderData(0, Qt::Horizontal, "标题");
    m_model->setHeaderData(1, Qt::Horizontal, "作者");
    m_model->setHeaderData(2, Qt::Horizontal, "时长");
    m_model->setHeaderData(3, Qt::Horizontal, "来源");

    // 2. 准备测试数据（二维数组或简单循环）
    QStringList titles = {"ats", "m3", "Sinos", "Sunset", "time"};
    QStringList authors = {"周杰伦", "张杰", "周杰伦", "薛之谦", "未定义"};

    for (int i = 0; i < titles.size(); ++i) {
        // 设置“标题”列
        m_model->setItem(i, 0, new QStandardItem(titles[i]));
        // 设置“作者”列
        m_model->setItem(i, 1, new QStandardItem(authors[i]));
        // 设置“时长”列
        m_model->setItem(i, 2, new QStandardItem("04:00"));
        // 设置“来源”列
        m_model->setItem(i, 3, new QStandardItem("本地"));
    }

    // 3. 关联到 View
    ui->tableView->setModel(m_model);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    // m_audioOutput->setVolume(0.2);

    QSettings settings;
    int lastVolume = settings.value("volume", 50).toInt();  // 读取 "volume" 键的值，如果找不到（第一次运行），默认设为 50
    // 应用到 UI 和音频输出
    ui->verticalSlider->setValue(lastVolume);
    m_audioOutput->setVolume(lastVolume / 100.0f);

    // 双击播放
    connect(ui->tableView, &QTableView::doubleClicked, this, [=](const QModelIndex &index){
        int row = index.row();
        playAudioByRow(row);
        is_play = true;
        ui->pushButton_play->setText("暂停");

        if (ui->verticalSlider->isVisible()){
            ui->verticalSlider->hide();
        }
    });

    // 当前音乐结束自动下一曲
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        // 当状态变为“媒体播放结束”时
        if (status == QMediaPlayer::EndOfMedia) {
            int currentRow = ui->tableView->currentIndex().row();
            int nextRow = currentRow + 1;

            // 判断是否还有下一首
            if (nextRow < m_model->rowCount()) {
                qDebug() << "当前首播放完毕，准备播放下一首：" << nextRow;
                playAudioByRow(nextRow);
            } else {
                qDebug() << "已经是最后一首了";
                playAudioByRow(0);
            }
        }
    });

    // 标签总时长变化（当切换歌曲时触发）
    connect(m_player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        ui->label_total->setText(formatTime(duration));
    });

    // 标签当前时长 监听当前播放进度变化（播放时每秒触发多次）
    connect(m_player, &QMediaPlayer::positionChanged, this, [=](qint64 pos) {
        ui->label_current->setText(formatTime(pos));
    });

    // 设置进度条的最大值（当歌曲加载完成或时长改变时）
    connect(m_player, &QMediaPlayer::durationChanged, this, [=](qint64 duration) {
        ui->horizontalSlider->setRange(0, duration); // 直接设为总毫秒数
    });

    // 实时更新进度条位置
    connect(m_player, &QMediaPlayer::positionChanged, this, [=](qint64 position) {
        if (!ui->horizontalSlider->isSliderDown()) {
            ui->horizontalSlider->setValue(position);
        }
    });

    // 音乐进度条鼠标拖拽
    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, [=](int position) {
        m_player->setPosition(position);
    });

    // 音乐进度条鼠标点击
    connect(ui->horizontalSlider, &QSlider::sliderPressed, this, [=](){
        int x = ui->horizontalSlider->mapFromGlobal(QCursor::pos()).x();
        double ratio = (double)x / ui->horizontalSlider->width();
        int targetPos = ratio * ui->horizontalSlider->maximum();
        m_player->setPosition(targetPos);
    });
}

Widget::~Widget()
{
    delete ui;
}


bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    // 监听鼠标按下事件
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        // 如果音量条当前是显示的，才需要判断是否隐藏
        if (ui->verticalSlider->isVisible()) {
            // 检查点击位置是否在【音量按钮】或【音量条】的矩形区域内
            // geometry() 获取控件相对于父窗口的位置和大小
            bool clickOnBtn = ui->pushButton_volume->geometry().contains(mouseEvent->pos());
            bool clickOnSlider = ui->verticalSlider->geometry().contains(mouseEvent->pos());

            // 如果点在了这两个地方之外，就隐藏
            if (!clickOnBtn && !clickOnSlider) {
                ui->verticalSlider->hide();
                // 注意：这里不要 return true，否则点击其他按钮的动作会被拦截掉（点不动其他按钮）
            }
        }
    }

    if (event->type() == QEvent::WindowDeactivate) {
        if (ui->verticalSlider->isVisible()) {
            ui->verticalSlider->hide();
        }
    }

    return QWidget::eventFilter(obj, event);
}


void Widget::on_pushButton_play_clicked()
{
    if(!is_play){
        qDebug() << "准备播放音乐";
        is_play = true;
        ui->pushButton_play->setText("暂停");
        m_player->play();
    }else{
        qDebug() << "准备暂停音乐";
        is_play = false;
        ui->pushButton_play->setText("播放");
        m_player->pause();
    }
}


void Widget::on_verticalSlider_valueChanged(int value)
{
    float volume = static_cast<float>(value) / 100.0f;
    m_audioOutput->setVolume(volume);

    // 实时保存音量设置
    QSettings settings;
    settings.setValue("volume", value);
}


void Widget::playAudioByRow(int row)
{
    if (row < 0 || row >= m_model->rowCount()) return; // 防越界

    // 1. 高亮选中这一行（视觉反馈）
    ui->tableView->selectRow(row);
    ui->tableView->setCurrentIndex(m_model->index(row, 0));

    // 2. 获取歌名并拼接路径（和你之前的逻辑一致）
    QString audio_name = m_model->data(m_model->index(row, 0)).toString();
    QString audio_path = "E:/Qt/ZAudios/" + audio_name + ".mp3";

    // 3. 播放
    m_player->setSource(QUrl::fromLocalFile(audio_path));
    m_player->play();
}


void Widget::on_pushButton_prev_clicked()
{
    int prevRow = ui->tableView->currentIndex().row() - 1;
    if (prevRow >= 0) {
        playAudioByRow(prevRow);
    }else{
        playAudioByRow(m_model->rowCount()-1);
    }
}


void Widget::on_pushButton_next_clicked()
{
    int nextRow = ui->tableView->currentIndex().row() + 1;
    if (nextRow < m_model->rowCount()) {
        playAudioByRow(nextRow);
    }else{
        playAudioByRow(0);
    }
}


void Widget::on_pushButton_volume_clicked()
{
    if (ui->verticalSlider->isHidden()) {
        ui->verticalSlider->show();
        // 建议：显示时把它置于顶层，防止被其他控件遮挡
        ui->verticalSlider->raise();
    } else {
        ui->verticalSlider->hide();
    }
}

