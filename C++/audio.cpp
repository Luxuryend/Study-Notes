void Widget::on_pushButton_test_clicked()
{
    // 在头文件声明
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;

    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(0.2);

    QString local_url = "E:\\ztestmusic\\m3.mp3";
    m_player->setSource(QUrl::fromLocalFile(local_url));
    m_player->play();
}
