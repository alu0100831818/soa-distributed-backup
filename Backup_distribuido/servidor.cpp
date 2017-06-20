/*
    Mayra Garcia
    Clase: Servidor.
    SOA.

    Esta clase se compone una lista de objetos que gestionan la conexion con distintos clientes
    orientada al envio multiple de distintas funtes, en este caso, solo se permite el envio de un
    unico cliente y la recepcion del resto.


*/


#include <QMessageBox>
#include "servidor.h"
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

//Funcion de tiempo.
void delay_1(int k){
    QTime dieTime= QTime::currentTime().addSecs(k);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//Constructor del servidor
Server::Server(QHostAddress host, quint16 port, QObject *parent)
    : QTcpServer(parent),
      ipp(host),
      puerto(port)
{

    directorio=QDir::home();
    directorio.mkdir("Servidor_BackUpD");
    directorio.cd("Servidor_BackUpD");

    clientes_esperados=50;  //espero muchos, la primera vez que se conecta cliente de envio, machaco valor
    clientes_conectados=0;
    cliente_conectado=0;
}
void Server::desconect(int i){
    //cuando uno de los sockets creados como origen se desconecta, liberamos la posicion funte.
    clientes_esperados=50;  //espero muchos, la primera vez que se conecta cliente de envio, machaco valor
    clientes_conectados=0;
    cliente_conectado=0;

    directorio=QDir::home();
    directorio.mkdir("Servidor_BackUpD");
    directorio.cd("Servidor_BackUpD");
   //
    QString a;
    if(i==0)
        a= "-------Se ha desconectado un cliente destino--------";
    else{
        Cliente=NULL;
        a="---------Se ha desconectado el Cliente Origen---------";
    }

    emit datos(a,1); //Datos que se van a escribir en la ventana
}

//Inicio y escucha del servidor
void Server::start()
{
    if ( this->listen(QHostAddress::Any, puerto) ){
        QString a;
        a= "CONEXION: Servidor iniciado en la ip: " ;
        a+= ipp.toString();
        a+= " Puerto: " ;
        a+= QString::number(puerto);

        emit datos(a,0); //inicio
        qDebug() << a;
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("No se puede iniciar el servidor..");
        msgBox.exec();
        return;
    }
}

//Entrada de una conexion
void Server::incomingConnection(qintptr handle)
{

    if((clientes_conectados<clientes_esperados)){
        clientes_conectados++;
        qDebug() << "incomingConnection = " << handle;
        SocketThread *thread = new SocketThread(&directorio,handle,&CLIENTES,&Datos_Cliente);

        connect(thread,SIGNAL(datos(QString,int)),this,SIGNAL(datos(QString,int)));
        connect(thread,SIGNAL(rango(int)),this,SIGNAL(rango(int)));
        connect(thread,SIGNAL(incremento(int)),this,SIGNAL(incremento(int)));
        connect(thread, SIGNAL(disconect(int)),this, SLOT(desconect(int)));
        connect(thread, SIGNAL(read(int)),this, SIGNAL(lectura(int)));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(thread, SIGNAL(lectura(int,int,QTcpSocket *)), this, SLOT(lectura_cliente(int,int,QTcpSocket *)));
        connect(this, SIGNAL(todos(QTcpSocket *)), thread, SLOT(todos(QTcpSocket *)));
        connect(this, SIGNAL(conected(int)), thread, SLOT(conected(int))); //cliente oriigen que se conecta
        connect(thread, SIGNAL(fin_transmision()), this, SLOT(adios()));
        thread->start();

    }
    else{
            QTcpSocket* m_socket = new QTcpSocket;
            m_socket = new QTcpSocket;
            m_socket->setSocketDescriptor(handle);
            m_socket->write("ocupado\n");
            emit datos("->>>>Se intentan conectar mas clientes de los esperados para envio \n Se les ha comunicado que el servidor está ocupado..",1);
            m_socket->close();
    }
    if (clientes_conectados>=clientes_esperados){
        QString a="CONEXION: Ha llegado una solicitud para transmitir datos \n       ->se ha aceptado, cliente: ";
        QHostAddress h=Cliente->peerAddress();
         a+= h.toString();
         a+= " Puerto: " ;
         a+= QString::number(Cliente->peerPort());
         emit datos(a,0);
        Cliente->write("aceptada\n");

    }

}

void Server::adios(){
    cliente_conectado=0;    //el cliente que enviaba datos, ha terminado
    clientes_esperados=50;
}



 void Server::lectura_cliente(int k, int cc, QTcpSocket * origen){
     if (k==1){
         cliente_conectado=1;
         emit conected(1);
         clientes_esperados=cc;
         clientes_conectados--;
         qDebug() << "cliente que quiere enviar paquetes" << cc << clientes_conectados;
         Cliente=origen;
     }
 }
Server::~Server(){}
//__________________________________________________________________________________________________________

//Cada uno de estos objetos creados gestiona una conexion con cada cliente, de tal manera que
//se tiene una lista con los destinatarios y es el que se conecta con origen
//quien reenvia sus datos a estos.

SocketThread::SocketThread(QDir* directorio,qintptr descriptor, QQueue<QPair<qintptr,QTcpSocket*>>* lista , QQueue<QPair<QByteArray, QString> > *Datos_Cliente, QObject *parent)
    : QThread(parent),
      m_socketDescriptor(descriptor),
      m_blockSize(0)
{
    t=0;
    AC=Datos_Cliente;
    Lista=lista;
    cliente_conectado=0;
    directorio_1=directorio;
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_socketDescriptor);
    emit read(1);
    connect(m_socket, SIGNAL(readyRead()),    this, SLOT(onReadyRead()),    Qt::DirectConnection);
    connect(m_socket, SIGNAL(disconnected()),    this, SLOT(onDisconnected()),    Qt::DirectConnection);

    QPair <qintptr,QTcpSocket*> b;
    b.first=m_socketDescriptor;
    b.second=m_socket;

    Lista->push_back(b);
    size=0;

}

SocketThread::~SocketThread()
{
    delete m_socket;
}


void SocketThread::run()
{

    host.first = m_socket->peerAddress().toString().split(":").takeLast();
    host.second =  m_socket->peerPort();
    //emit cliente(host.second);

    connect(m_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    exec();
}

void SocketThread::todos(QTcpSocket * a){

       // send("","aceptada\n",a);
}



void SocketThread::leer(void){


}



void SocketThread::send(QByteArray q, QString filename, QTcpSocket* a){

    if(a!=NULL){
        m_socket=a;
    }
    QString prub=filename.split(" ").takeFirst();
    int f =QString::compare(filename, "aceptada\n", Qt::CaseInsensitive);
    int b =QString::compare(filename, "wait\n", Qt::CaseInsensitive);
    int by =QString::compare(prub, "bytes", Qt::CaseInsensitive);
    int s =QString::compare(filename, "ocupado\n", Qt::CaseInsensitive);
    int d =QString::compare(filename, "desconet\n", Qt::CaseInsensitive);
    int ff=QString::compare(filename, "fin\n", Qt::CaseInsensitive);
    int e=QString::compare(filename, "envio\n", Qt::CaseInsensitive);
    if(f==0){
        //se acepta la solicitud del cliente
        m_socket->write("aceptada\n");
    }
    else{
        if(b==0){
            //el cliente origen espera a que haya mas clientes destino conectados
            QString g="CONEXION: Ha llegado una solicitud para transmitir datos \n       ->se ha aceptado, cliente: ";
            QHostAddress h=m_socket->peerAddress();
            g+= h.toString();
            g+= " Puerto: " ;
            g+= QString::number(m_socket->peerPort());
            emit datos(g,0);
            emit datos("A la espera de que se conecten los clientes solicitados por Origen de datos.",2);
            m_socket->write("wait\n");
        }
        else{
            if(s==0){
                //el servidor no puede conectar a mas clientes origen
                emit datos("Se han recibido mas solicitudes de conexion como Origen de datos, se ha rechazado al cliente: " + m_socket->peerAddress().toString() + " Puerto: "+ QString::number(m_socket->peerPort()),1);
                m_socket->write("ocupado\n");
            }
            else{
               if(by==0){
                   //s= s.split(QString::number(Cliente_o->peerPort())).takeLast();
                   QByteArray tam; QString dd;
                   tam.append(QString("b "));  //incluimos b
                   tam.append(QString::number(q.toInt())); //inlcuimos tamaño
                   tam.append(" "); //incluimos espacio
                   //tam.append("/" + filename_1.split("/").takeLast()); //incluimos fichero
                   //QString t=filename.split(" ").takeLast().split(filename_1).takeLast(); //mira si en el fichero hay alguna / ->dire?
                   dd.append(filename.split(QString::number(Cliente_o->peerPort())).takeLast());
                   if((dd.size()>0)&&(dd[0]!="/"))
                       tam.append("/" );
                   qDebug() <<"t: " << t <<" tam:" << tam;
                   tam.append(dd);
                   tam.append("\n");
                   m_socket->write(tam);
                   qDebug() <<"bytes: "<< tam;

               }
               else{
                   if(d==0){
                       //desconectar

                   }
                   else{
                       if(ff==0){
                           //fin
                            m_socket->write("fin\n");
                       }
                       else{
                           if(e==0){
                                //se le dice cuantos ficheros se van a eenviar, para el incremento
                                QByteArray envi="e ";
                                envi.append(q);
                                envi.append("\n");
                                m_socket->write(envi);
                           }
                           else{
                               //se reenvia un dato, tipo el fichero
                                qDebug() <<"aqui se reenvian ficheros----";

                                QFile* m_file= new QFile(filename);
                                 if ((!m_file->open(QIODevice::ReadOnly)) ) {
                                     qDebug() << "No puedo abrir el fichero";
                                     return;
                                 }
                                 int ss=m_file->size();
                                 if(ss>1000)
                                      delay_1(1);
                                  QString filee="bytes ";
                                  filee.append(filename);
                                  send(QByteArray::number(ss),filee,m_socket);
                               qint64 sise=m_file->size(); qint64 tr=sise;
                                  int x=0;
                              QByteArray read;
                              qint64 es=50000;
                              if(ss>=10000000)
                                  es=10000000;
                              qDebug() << "tamaño del socket: " << m_socket->size();
                               while(m_socket->isWritable())
                                        {

                                           read.clear();
                                           sise=sise-10000;
                                           x=10000+sise;
                                            if(sise>0)
                                            {
                                                if(sise<1000)
                                                     read = m_file->read(tr);
                                                  else{
                                                    if(sise>100000)
                                                      read = m_file->read(100000);
                                                    else
                                                       read = m_file->read(10000);
                                                }
                                            }
                                            else{
                                                if(x>0)
                                                  read = m_file->read(x);
                                            }

                                            qDebug() << "Read : " << read.size();
                                            if(read.size()<=0){
                                              break;
                                            }
                                            es+=m_socket->write(read);
                                            qDebug() << "Written : " << read.size();

                                            if(read.size()>es){
                                                m_socket->waitForBytesWritten();

                                            }


                                  }
                                 m_file->close();



                           }


                       }
                   }
               }
           }
        }

    }

}

void SocketThread::Arbol(QString ruta_fichero,QString puerto ){

    QString ruta= directorio_1->absolutePath();
     qDebug() << "ruta_dir_escribir: " << ruta;

    //creo la carpeta del cliente que me ha enviado datos, en base a su opuerto de conexion!
    directorio_1->mkdir(puerto);
    directorio_1->cd(puerto);

    QStringList dirs=ruta_fichero.split("/");
    for(int i=0;i<dirs.size()-1;i++){
         directorio_1->mkdir(dirs[i]);
         directorio_1->cd(dirs[i]);
    }

    directorio_1->setPath(ruta);

}


void SocketThread::onReadyRead()
{
        QByteArray linea;
        QString c; QString x;
        if(m_socket->canReadLine()) //Leo la primera linea que se ma transferido
          if(size==0){
                linea=m_socket->readLine();
                c.append(linea);
                x=c.split(" ").takeFirst();
                qDebug() <<"Lo que ha lleagdo"<< c;
          }

            if(x=="s"){
                    //comprobar si no hay usuarios origen conectados ya
                if(cliente_conectado==0){
                    Cliente_o=m_socket;
                    int cc=c.split("\n").takeFirst().split(" ").at(1).toInt();
                    emit lectura(1,cc,m_socket);
                    cliente_conectado=1;
                    Lista->pop_back();

                    qDebug() <<"Clientes que se esperan: "<< cc;
                    emit datos("->Clientes que se esperan: "+ QString::number(cc),2);
                    if(cc==Lista->size()){
                        //t=1;
                        QString name=directorio_1->absolutePath() + "/" + QString::number(m_socket->peerPort());
                        filename_1=name;
                       qDebug() <<"Me ha lleagdo una solicitud y ha sido aceptada...";
                    }
                    else    //si no estan todos los clientes le decimos que espere
                        send("","wait\n",NULL);

                }
                else{
                      send("","ocupado\n",NULL);
                }
            }
            else{
                if(x=="b"){
                    //numero de bytes que llegan
                    size=c.split("\n").takeFirst().split(" ").at(1).toInt();
                    actual.second=c.split("\n").takeFirst().split(" ").at(2);

                    QString v;

                    if(archivos_actuales==1){
                        v.append("/" + actual.second);
                    }
                    else{
                        v.append(actual.second);
                    }
                    //emit read(actual.second);
                    Arbol(v,QString::number(m_socket->peerPort()));

                    actual.first=QByteArray::number(size);
                }
                else{
                    if(x=="f"){
                        size=0;
                           emit datos("------Ha finalizado el envio del cliente orige, comienza el reenvio hacia lientes, destino..",2);
                          //vamos a reenviar a los clientes
                          envio();

                    }
                    else{
                        if(x=="d"){
                             qDebug() <<"cliente que se desconecta.................";
                        }
                        else{
                            if(x=="e"){
                                archivos_actuales=c.split("\n").takeFirst().split(" ").at(1).toInt();
                                emit datos("->Numero de archivos que se esperan del cliente: "+ QString::number(archivos_actuales),2);
                                emit rango(archivos_actuales);
                            }
                            else{
                                if(x=="a"){

                                }
                                else{
                                     //esta ruta es valida para linux, no es una solucion buena para otros sistemas...
                                     //deberia vernir asi: /home/...  (actual.second)
                                    QString name=directorio_1->absolutePath() + "/" + QString::number(m_socket->peerPort());

                                    name.append(actual.second);
                                    QFile file(name);
                                    if (!file.open(QIODevice::WriteOnly | QIODevice::ReadWrite))
                                         qDebug() << "No se abre el fichero...";
                                    else{
                                        int ssize=0;
                                        int o=size;
                                        qint64 es=50000;
                                        if(size>=1000000)
                                            es=1000000;
                                         while(ssize < size){
                                             if(size>es){
                                                m_socket->waitForReadyRead();
                                             }
                                             if(m_socket->bytesAvailable()>0){

                                                 QByteArray c;
                                                 int k=m_socket->bytesAvailable();
                                                 if(k<o){
                                                     c=m_socket->read(k);
                                                     file.write(c);
                                                     ssize+=c.size();
                                                     o-=c.size();
                                                     qDebug() <<"o: "<< o << " ssize: " << ssize;
                                                 }
                                                 else{
                                                     c=m_socket->read(o);
                                                     file.write(c);
                                                     ssize+=c.size();
                                                     o-=c.size();
                                                     qDebug() <<"o: "<< o << " ssize: " << ssize;
                                                     break;
                                                 }

                                             }


                                         }


                                        size=0;

                                        emit datos("*Archivo que se ha enviado: "+ actual.second+ QString::number(file.size()),2);
                                        file.close();
                                        actual.second=name; //ruta absoluta en el servidor
                                        AC->append(actual);

                                        emit incremento(1);
                                    }
                                }


                            }
                       }
                    }
                }
            }
            if(m_socket->bytesAvailable()>0){
                onReadyRead();
            }
}
void  SocketThread::conected(int x){
    cliente_conectado=x;
}

void SocketThread::onDisconnected()
{
    m_socket->close();
    if(m_socket==Cliente_o)
        emit disconect(1);
    else
        emit disconect(0);
}


 void SocketThread::envio(){
     //enviamos a los clientes que estan en la lista todos los ficheros que nos han llegado.
     for(int j=0;j<Lista->size();j++){
         send(QByteArray::number(AC->size()),"envio\n",Lista->operator [](j).second);
     }

    for(int j=0;j<Lista->size();j++){
        for(int i=0;i<AC->size();i++){
              QString s=AC->operator [](i).second;
              send("",s,Lista->operator [](j).second);
        }
    }

    for(int j=0;j<Lista->size();j++){
        send("","fin\n",Lista->operator [](j).second);
    }
 }
