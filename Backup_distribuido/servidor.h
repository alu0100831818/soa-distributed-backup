#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
#include <QFile>
#include <QThread>
 #include <QDataStream>
#include <QDir>
 #include <QMutex>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QHostAddress ipp,
                    quint16 puerto,
                    QObject *parent   = 0);
    ~Server();

public slots:
    void start();
    void lectura_cliente(int k, int cc, QTcpSocket* origen);
    void adios();
    void desconect(int i);
    void todos();
signals:
    void datos(QString,int);
    void lectura(int);
    void conexion(int);
    void todos(QTcpSocket *);
    void rango(int);        //progressbar
    void incremento(int);
    void conected(int);
protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

private:
    QMutex* mutex;
    QString* cadena1;
    QDir directorio;
    qintptr conexion_actual;
    QHostAddress ipp;
    int cliente_conectado;
    quint16      puerto;
    QTcpSocket *  Cliente; //el que envia los datos
    QQueue<QPair<qintptr,QTcpSocket*>> CLIENTES; //lista de clientes
    QQueue <QPair<QByteArray,QString>>Datos_Cliente;
    int clientes_esperados;
    int clientes_conectados;

};

//____________________________________________


class SocketThread : public QThread
{
    Q_OBJECT
public:
    SocketThread(QDir* directorio, qintptr descriptor, QQueue<QPair<qintptr, QTcpSocket *> > *lista, QQueue <QPair<QByteArray,QString>>*Datos_Cliente, int pp, QObject *parent = 0);
    ~SocketThread();

    void leer(void);
    void leer2();
    void send(QByteArray q,QString filename,QTcpSocket* a);
    void Arbol(QString ruta_fichero, QString puerto);
public slots:

    void envio();
    void todos(QTcpSocket *a);
    void conected(int);
protected:
    void run() Q_DECL_OVERRIDE;

signals:

    void desc_ccdos();
    void datos(QString,int);
    void onFinishRecieved();
    void lectura(int a,int cc,QTcpSocket *);
    void cliente(int );
    void fin_transmision();
    void read(int);
    void rango(int);        //progressbar
    void incremento(int);
    void disconect(int i);
    void todos();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
     QString filename_1;
    QString* cadena1;
    QDir* directorio_1;
    QByteArray sd;
    quint64 sender_block_ = 0;
    int size;
    int cliente_conectado;
    int archivos_actuales;
    QPair<QString, int> host;
    QQueue<QPair<qintptr,QTcpSocket*>>* Lista;
    QQueue <QPair<QByteArray,QString>>* AC;  //archivos enviados por el cliente
    QQueue<QString> arbol;
    qintptr     m_socketDescriptor;
    qintptr     ORI;
    QTcpSocket *m_socket;
    QTcpSocket *socket;
    qint32      m_blockSize;
    QPair<QByteArray,QString> actual;
    QTcpSocket *Cliente_o;
    int t;
    qint16 socketDescriptor_1;
    qint16 socketDescriptor_2; //del cliente
};
#endif // SERVIDOR_H
