#ifndef CLIENTE_H
#define CLIENTE_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QFileDialog>
#include <QFile>
#include <QQueue>

class SocketTest : public QObject
{
    Q_OBJECT
public:
    explicit SocketTest(int port, QString ipp, int origen, int dir, int clientes, QObject *parent=0);

    ~SocketTest();
signals:
    void rango(int);
    void incremento(int);
    void datos(QString,int);
    void b_3();
    void inicio_cliente();
public slots:
    void Test();
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void write(QString filename);
    void send(QByteArray q, QString filename);
    void leer();
    void Arbol(QString ruta_fichero,QString puerto );

private:
    int  archivos_actuales;
    QDir* directorio_destino;
    int size;
    QObject *parent_;
    int directorio;
    QTcpSocket *socket;
    int origen;
    QFile * m_file;
    QString FFilename;
    int clientes_enviar;
    QString ruta;  //rruta de almacenamiento del cliente destino
    QQueue<QPair<QByteArray,QString>> Datos;
    QString IP;
    int PORT;
    QString filename_1; //contiene el dir a enviar por el cliente origen
    QPair<QByteArray,QString> actual;
};

#endif // CLIENTE_H
