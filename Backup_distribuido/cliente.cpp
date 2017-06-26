/*
    Mayra Garcia
    Clase: Cliente
    SOA.

   Esta clase se compone de un tipo de objeto que se conecta a un servidor en base a un rol: Fuente de datos
   o destino de estos.


*/
#include "cliente.h"
#include "QDirIterator"
#include <QMessageBox>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

//Funcion de tiempo.
void delay(int k)
{
    QTime dieTime= QTime::currentTime().addSecs(k);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

SocketTest::SocketTest(int port, QString ipp, int Origen, int dir, int clientes,QObject *parent) :
    QObject(parent)
{

    //directorio=0;
    clientes_enviar=clientes;
    directorio=dir;
    origen = Origen;
    //
    IP=ipp;
    PORT=port;
}


SocketTest::~SocketTest(){


}

void SocketTest::Test()
{
    size=0;
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    qDebug() << "Connecting,..";
    QString dat="Conexion a Servidor: ";
    dat.append(IP);
    dat.append("-");
    dat.append(QString::number(PORT));
    emit datos(dat,0);
    socket->connectToHost(IP, PORT);
    socket->waitForConnected();
    if((socket->state() != QAbstractSocket::ConnectedState)){
        QMessageBox msgBox;
        msgBox.setText("No se puede conectar con el servidor, intentelo de nuevo...");
        msgBox.exec();
        //HAY QUE HABILITAR LA VENTANA DE NUEVO___________________________________________________________________________
        //________________________________________________________________________________________________________________
        delete socket;
        return;
    }

    if(origen==1){          //SI EL CLIENTE ES EL ORIGEN!!!!

        send("","solicitud");

    }
    else{
        //clliente destino
        ruta= QFileDialog::getExistingDirectory(NULL,tr("CLIENTE-DESTINO"),"/");
        qDebug() << "ruta: "<< ruta;
        ruta.replace(" ","-");
        emit datos("->Ruta destino seleccionada: " + ruta,0);
        directorio_destino = new QDir(ruta);
        send("","destino");
//        directorio_destino->mkdir(QString::number(PORT));
//        directorio_destino->cd(QString::number(PORT));


    }


}

void SocketTest::connected()
{
    qDebug() << "Connected!";

}



void Dir_recursivo(QStringList* Archivos,QString filename){ //leer arbol de directorio y
                                                           //generar lista de archivos para leer
            //archivos contenidos un directorio
    QDirIterator it(filename, QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        Archivos->push_back(it.next());
    }

}

void SocketTest::send(QByteArray q,QString filename){
    QByteArray block;
    QString prub=filename.split(" ").takeFirst();
    int f =QString::compare(prub, "fin", Qt::CaseInsensitive);
    int v =QString::compare(prub, "envio", Qt::CaseInsensitive);
    int b =QString::compare(prub, "bytes", Qt::CaseInsensitive);
    int s =QString::compare(prub, "solicitud", Qt::CaseInsensitive);
    int d =QString::compare(prub, "desconet", Qt::CaseInsensitive);
    int a =QString::compare(prub, "arbol", Qt::CaseInsensitive);
    int n =QString::compare(prub, "destino", Qt::CaseInsensitive);

    block.append(q);
    if(b==0){ //bytes que se van a enviar
        QByteArray tam;
        QString file_copia=filename_1;
        file_copia.replace(" ","-");
        tam.append(QString("b "));  //incluimos b
        tam.append(QString::number(q.toInt())); //inlcuimos tamaño
        tam.append(" "); //incluimos espacio
        tam.append("/" + file_copia.split("/").takeLast()); //incluimos fichero
        QString t=filename.split(" ").takeLast().split(file_copia).takeLast();
        if((!t.contains("/"))&&(t.size()>0))
            tam.append("/" );
        qDebug() <<"t: " << t <<" tam:" << tam;
        tam.append(t);
        tam.append("\n");
        socket->write(tam);
        qDebug() <<"bytes: "<< tam;

    }
    else{
        if(s==0){
            //solicitud de transferencia
            emit datos("-Se ha enviado una solicitud de conexion al servidor indicado, en espera de su respuesta..",2);
            QByteArray a="s"; a.append(" ");
            a.append(QByteArray::number(clientes_enviar)); // s numero_cc\n
            a.append("\n");

            socket->write(a); //solicitud, usuarios, archivos
        }
        else{
            if(d==0){//desconectarse

                    qDebug() <<"me desconecto..";
            }
            else{
               if(f==0){ //fin de transmision
                    socket->write(QByteArray("f \n"));
                    emit datos("->Se ha finalizado la transmision de datos hacia el servidor..",1);
                    qDebug() << "envio fin";
               }
               else{
                   if(v==0){
                       QByteArray a="e"; a.append(" ");
                       a.append(q); // e numero_archivos
                       a.append("\n");
                        socket->write(a);

                        emit datos("Numero de archivos que se van a enviar: "+ QString(q),2);
                        emit datos("Carpeta o archivo que se va a enviar: "+ filename_1,2);
                   }
                  else{
                       if(a==0){
                           //filename_1 contiene la ruta elegida por el cliente
                           QString ru="/";

                           QString ru_1;
                           ru_1.append(q);
                           ru_1=ru_1.split(filename_1).takeLast();

                           ru.append(ru_1);
                           //envio el nodos del arbol
                           QByteArray s="a"; s.append(" ");

                           //filename contiene toda la ruta, pero solo vamos a enviar un trozo
                           s.append(ru); // "a /ruta/dir/fic.txt\n"
                           s.append("\n");
                            socket->write(s);

                       }
                      else{
                           if(n==0){
                               //cliente destino envia mensaje:
                                    socket->write("d \n");
                           }
                           else{
                                  QFile* m_file= new QFile(filename);
                                   if ((!m_file->open(QIODevice::ReadOnly)) ) {
                                       qDebug() << "No puedo abrir el fichero";
                                       return;
                                   }
                                   int ss=m_file->size();
                                   if(ss>1000)
                                        delay(1);
                                    QString filee="bytes ";
                                    filee.append(filename);
                                    send(QByteArray::number(ss),filee);
                                 qint64 sise=m_file->size(); qint64 tr=sise;
                                    int x=0;
                                QByteArray read;
                                qint64 es=50000;
                                if(ss>=10000000)
                                    es=10000000;
                                qDebug() << "tamaño del socket: " << socket->size();
                                 while(socket->isWritable())
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
                                              es+=socket->write(read);
                                              qDebug() << "Written : " << read.size();

                                              if(read.size()>es){
                                                  socket->waitForBytesWritten();

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

void SocketTest::write(QString filename)
{

    QByteArray q;
    QFileInfo path(filename);
    //QByteArray n;

    if(path.isFile()){
        emit rango(1);
         qDebug() <<filename.split("/").takeLast();
         send("", filename);
         emit incremento(1);

    }
    if(path.isDir()){

         QStringList* Archivos= new QStringList;
         //QStringList* arbol= new QStringList;

            Dir_recursivo(Archivos,filename);

            send(QByteArray::number(Archivos->size()),"envio ");
            emit rango(Archivos->size());
            while(!(Archivos->empty())){
                emit incremento(1);
                send("",Archivos->back());
                Archivos->pop_back();

            }
     }
    send("","fin ");
}

void SocketTest::disconnected()
{
    QMessageBox msgBox;
    msgBox.setText("Se ha desconectado del servidor!!");
    msgBox.exec();
    socket->write(QByteArray("c \n"));
    //qDebug() << "Disconnected!";
}

void SocketTest::bytesWritten(qint64 bytes)
{
    //qDebug() << "We wrote: " << bytes;
}

void SocketTest::leer()
{

}

void SocketTest::readyRead()
{
    QByteArray linea;
    QString c;
    if(socket->canReadLine()) //Leo la primera linea que se ma transferido
      if(size==0){
            linea=socket->readLine();
            c.append(linea);
            qDebug() <<"Lo que ha llegado"<< c;
      }
        QString fileName;
        fileName=c.split("\n").takeFirst();
        QString fil= fileName.split(" ").takeFirst();
        int k=QString::compare(fileName,"aceptada", Qt::CaseInsensitive);
        int s=QString::compare(fileName,"ocupado", Qt::CaseInsensitive);
        int w=QString::compare(fileName,"wait", Qt::CaseInsensitive);
        int m=QString::compare(fileName,"fin", Qt::CaseInsensitive);  //fin de transmision desde el servidor
        int b=QString::compare(fil,"b", Qt::CaseInsensitive);
        int x=QString::compare(fil,"e", Qt::CaseInsensitive);
     if(k==0){
         if(directorio==1){
               filename_1= QFileDialog::getExistingDirectory(NULL,tr("ORIGEN: Transferir Directorio"),"/");


         }
         else{
               filename_1= QFileDialog::getOpenFileName(NULL,tr("ORIGEN: Transferir fichero"), "/", tr("*"));
               send(QByteArray::number(1),"envio ");

        }
        qDebug() << "Reading..."<<fileName ;
        write(filename_1);
     }
     else{
             if(s==0){
                 QMessageBox msgBox;
                 msgBox.setText("El servidor tiene su cupo de clientes saturado, intentelo mas tarde..");
                 msgBox.exec();
                 emit disconnected();
                 emit datos("Conexion imposible, el servidor está ocupado atendiendo a otro cliente...",0);
                 //emit b_3();
             }
             else{
                 if(w==0){
                     QMessageBox msgBox;
                     msgBox.setText("No hay clientes suficientes, esperando a conexion....");
                     msgBox.exec();
                     emit datos("No hay clientes suficientes, esperando a conexion....",2);
                 }
                 else{
                     if(m==0){
                         //fin
                         QMessageBox msgBox;
                         msgBox.setText("El servidor ha terminado de realizar el envio..");
                         msgBox.exec();
                        emit datos("El servidor ha terminado de realizar el envio..",2);
                         //emit disconnected();
                         emit b_3();
                         //emit disconnected();
                     }
                     else{
                         if(x==0){
                             archivos_actuales=c.split("\n").takeFirst().split(" ").at(1).toInt();
                             qDebug() << "El numero de archivos que va a enviar origen es: "<< archivos_actuales;
                             emit datos("->Numero de archivos que se esperan del servidor: "+ QString::number(archivos_actuales),2);
                             emit rango(archivos_actuales);
                         }
                         else{
                             if(b==0){
                                 //me llega el nombre del archivo, el tamaño.. etc
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
                                 Arbol(v,QString::number(socket->peerPort()));

                                 actual.first=QByteArray::number(size);
                                 qDebug() <<"Tamaño de fichero que llega: "<<size;
                             }
                             else{

                                 //esta ruta es valida para linux, no es una solucion buena para otros sistemas...
                                 //deberia vernir asi: /home/...  (actual.second)
                                QString name=directorio_destino->absolutePath() + "/" + QString::number(socket->peerPort());

                                name.append(actual.second);
                                qDebug() <<"ruta destino fichero: " << name;
                                QFile file(name);
                                if (!file.open(QIODevice::WriteOnly | QIODevice::ReadWrite))
                                     qDebug() << "No se abre el fichero...";
                                else{
                                    qDebug() <<"creando fichero.................";

                                    qDebug() <<"size: " << size;
                                    int ssize=0;
                                    int o=size;
                                    qint64 es=50000;
                                    if(size>=1000000)
                                        es=1000000;
                                     while(ssize < size){
                                         if(size>es){
                                            socket->waitForReadyRead();
                                         }
                                         if(socket->bytesAvailable()>0){

                                             QByteArray c;
                                             int k=socket->bytesAvailable();
                                             if(k<o){
                                                 c=socket->read(k);
                                                 file.write(c);
                                                 ssize+=c.size();
                                                 o-=c.size();
                                                 qDebug() <<"o: "<< o << " ssize: " << ssize;
                                             }
                                             else{
                                                 c=socket->read(o);
                                                 file.write(c);
                                                 ssize+=c.size();
                                                 o-=c.size();
                                                 qDebug() <<"o: "<< o << " ssize: " << ssize;
                                                 break;
                                             }

                                         }


                                     }


                                    size=0;

                                    emit datos("*Archivo que se ha enviado: "+ actual.second +" "+ QString::number(file.size()),2);
                                    file.close();
                                    //directorio_1->absolutePath() + "/" + QString::number(m_socket->peerPort())+ actual.second
                                    actual.second=name; //ruta absoluta en el servidor
                                    //AC->append(actual);
                                    qDebug() <<"cerrando fichero.................";
                                    emit incremento(1);
                                }


                             }


                         }

                    }

                 }
             }
         }

     if(socket->bytesAvailable()>0){
         readyRead();
     }
}

void SocketTest::Arbol(QString ruta_fichero,QString puerto ){

    QString ruta= directorio_destino->absolutePath();
     qDebug() << "ruta_dir_escribir: " << ruta;

    //creo la carpeta del cliente que me ha enviado datos, en base a su opuerto de conexion!! OJO CON ESTE VALOR, SE PUEDE REPETIR
    directorio_destino->mkdir(puerto);
    directorio_destino->cd(puerto);

    QStringList dirs=ruta_fichero.split("/");
    for(int i=0;i<dirs.size()-1;i++){
         dirs[i].replace(" ","-");
         directorio_destino->mkdir(dirs[i]);
         directorio_destino->cd(dirs[i]);
    }

    directorio_destino->setPath(ruta);

}

