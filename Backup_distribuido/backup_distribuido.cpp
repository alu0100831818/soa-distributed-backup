#include "backup_distribuido.h"
#include "ui_backup_distribuido.h"
#include <QFileDialog>
#include <QMessageBox>


Backup_distribuido::Backup_distribuido(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Backup_distribuido)
{
    ui->setupUi(this);
    servidor=NULL;
    cliente=NULL;
    //Inicio de variables para contemplar la posibilidad del servicio
    Servidor=200;
    Cliente_origen=200;
    Cliente_destino=200;
    t=0; //numero de archivos que se han enviado y rango para el progressbar
    //tab inicial
    on_tabWidget_tabBarClicked(0);

    //iniciacion de los tabs
    ui->tabWidget->setTabText(0, "Cliente");
    ui->tabWidget->setTabText(1,"Servidor" );
    ui->tabWidget->setTabText(2, "Datos");
    ui->textEdit->setReadOnly(true);

    ui->pushButton_6->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
    ui->pushButton_7->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));

    directorio=0;
    clientes_enviar=0;
    ui->progressBar->setOrientation(Qt::Horizontal);
    Barra_de_progreso(0);

}

void Backup_distribuido::Barra_ranfo(int range){
    qDebug() << "Me ha llegado el rango: " << range;
    //range sera el numero de archivos a enviar por cliente o servidor
    //EN caso de ser cliente origen, sera numero de archivos a recibir
    ui->progressBar->setRange(0,range);
}

void Backup_distribuido::Barra_de_progreso(int p){
    //cambiamos el valor de la barra
    qDebug() << "Me ha llegado el incremento:  " <<p;
    t=t+p;
    ui->progressBar->setValue(t);
}
Backup_distribuido::~Backup_distribuido()
{
    delete ui;
}


void Backup_distribuido::on_tabWidget_tabBarClicked(int index)
{
    origen=0;
    switch (index)
    {
    case 0:
        ui->textEdit->clear();
        ui->textEdit->setTextColor("red");
        ui->textEdit->insertPlainText("Backup-Distribuido: \n");
        ui->textEdit->setTextColor("black");
        break;
    case 1:
           if(Servidor==1){
               QHostAddress A(ip);
               ui->tab->setEnabled(false);
               servidor= new Server(A,puerto);
                ui->label_7->setStyleSheet("QLabel { background-color : white; color : magenta; }");
                ui->label_7->setText("Log de Datos: Servidor");
                //connect(servidor,SIGNAL(lectura(int)),this,SLOT(datos(int)));
                connect(servidor,SIGNAL(rango(int)),this,SLOT(Barra_ranfo(int )));
                connect(servidor,SIGNAL(incremento(int)),this,SLOT(Barra_de_progreso(int)));
                connect(servidor,SIGNAL(conexion(int )),this,SLOT(datos_cliente(int)));
                connect(servidor,SIGNAL(datos(QString,int)),this,SLOT(Datos(QString,int)));
                servidor->start();
                ui->Inicio->setEnabled(false);
                ui->tab->setEnabled(false);

          }
           else{
               if(Cliente_origen==1){   //envia los datos
                    ui->Inicio->setEnabled(false);
                    ui->label_7->setStyleSheet("QLabel { background-color : white; color : green; }");
                    ui->label_7->setText("Log de Datos: Cliente Origen");
                     origen=1;
                      cliente= new SocketTest(puerto, ip, origen,directorio,clientes_enviar,filename_1,this);
                      connect(cliente,SIGNAL(rango(int)),this,SLOT(Barra_ranfo(int )));
                      connect(cliente,SIGNAL(incremento(int)),this,SLOT(Barra_de_progreso(int)));
                      connect(cliente,SIGNAL(datos(QString,int)),this,SLOT(Datos(QString,int)));
                      connect(cliente,SIGNAL(reenvio_botones()),this,SLOT(activa_inicio()));

                      //connect(cliente,SIGNAL(inicio_cliente()),this,SLOT(activa_inicio()));
                      cliente->Test();
                      ui->Inicio->setEnabled(false);
                      ui->tab->setEnabled(false);


               }
               else{
                   if(Cliente_destino==1){
                       //RECIBE INFORMACION
                       ui->Inicio->setEnabled(false);
                       ui->label_7->setStyleSheet("QLabel { background-color : white; color : blue; }");
                        ui->label_7->setText("Log de Datos: Cliente Destino");
                       origen=0;
                       filename_1="";
                       cliente= new SocketTest(puerto, ip, origen,directorio,clientes_enviar,filename_1,this);
                       connect(cliente,SIGNAL(rango(int)),this,SLOT(Barra_ranfo(int )));
                       connect(cliente,SIGNAL(incremento(int)),this,SLOT(Barra_de_progreso(int)));
                       connect(cliente,SIGNAL(datos(QString,int)),this,SLOT(Datos(QString,int)));
                      // connect(cliente,SIGNAL(inicio_cliente()),this,SLOT(activa_inicio()));
                       cliente->Test();
                   }
                }
           }
        break;
    }

}
void Backup_distribuido::close_()
{
    this->close();
}
void Backup_distribuido::datos_cliente(int a){
    ui->textEdit->insertPlainText("cliente: ");
    ui->textEdit->insertPlainText(QString::number(a));
    ui->textEdit->insertPlainText(" \n");
    QPair<QHostAddress,int> client;
    client.second=a;

}




void Backup_distribuido::on_pushButton_clicked()
{

    //boton de cliente
    puerto=ui->lineEdit_2->text().toInt();
    ip=ui->lineEdit_3->text();
    clientes_enviar=ui->lineEdit->text().toInt();   //si el cliente es origen, tomamos el numero de clientes a enviar
    ui->tabWidget->setCurrentIndex(2);
    on_tabWidget_tabBarClicked(1);



}

void Backup_distribuido::on_pushButton_2_clicked()
{
    //boton de servidor
    if(Servidor==0||Servidor==200)
        Servidor=1;
    else
        Servidor=0;

    puerto=ui->lineEdit_4->text().toInt();  //LEEMOS DATOS DE LA CORRESPONDIENTE PESTAÑA
    ip=ui->lineEdit_5->text();
     ui->tabWidget->setCurrentIndex(2);
    on_tabWidget_tabBarClicked(1);      //INICIAMOS AL SERVIDOR
    //NOS POSICIONAMOS EN EL TAB DE INFORMACIONDEL ENVIO

}

void Backup_distribuido::on_radioButton_clicked()
{
    //cliente origen

    if(Cliente_origen==0||Cliente_origen==200){
        Cliente_origen=1;

    }
    else{
        Cliente_origen=0;
    }
}

void Backup_distribuido::on_pushButton_3_clicked()
{
    //desconectar del servidor o del cliente
    QMessageBox msgBox;

    msgBox.setText("...");
    if(Servidor==1){
        Servidor=0;
        if(servidor!=NULL){

            delete servidor;
           servidor=NULL;}

    }
    else{
        if(cliente != NULL){
            cliente->disconnected();
            cliente=NULL;
        }


    }

    this->close();
}


void Backup_distribuido::on_checkBox_clicked()
{
    if(directorio==0)
        directorio=1;
    else
        directorio=0;
}

void Backup_distribuido::on_radioButton_2_clicked()
{
    //cliente destino
    if(Cliente_destino==0|| Cliente_destino==200)
            Cliente_destino=1;  //el cliente destino se desactiva

    else
        Cliente_destino=0;
}

void Backup_distribuido ::Datos(QString a, int b){

    ui->textEdit->setTextColor("black");
    switch (b)
    {
        case 0:
            ui->textEdit->setTextColor("darkred");
        break;
        case 1:
            ui->textEdit->setTextColor("darktellow");
        break;
    }
    ui->textEdit->append(a);

}



void Backup_distribuido::on_pushButton_7_clicked() //fichero
{
    filename_1= QFileDialog::getOpenFileName(NULL,tr("ORIGEN: Transferir fichero"), "/", tr("*"));
    directorio=0;
}

void Backup_distribuido::on_pushButton_6_clicked() //directorio
{
    filename_1= QFileDialog::getExistingDirectory(NULL,tr("ORIGEN: Transferir Directorio"),"/");
    directorio=1;
}
