#ifndef BACKUP_DISTRIBUIDO_H
#define BACKUP_DISTRIBUIDO_H

#include <QMainWindow>
#include "cliente.h"
#include "servidor.h"

namespace Ui {
class Backup_distribuido;
}

class Backup_distribuido : public QMainWindow
{
    Q_OBJECT

public:
    explicit Backup_distribuido(QWidget *parent=0);
    ~Backup_distribuido();
public slots:
    void Datos(QString a,int b);
    void close_();

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_pushButton_clicked();
    void datos_cliente(int a);

    void on_pushButton_2_clicked();

    void on_radioButton_clicked();

    void on_pushButton_3_clicked();


    void on_checkBox_clicked();

    void on_radioButton_2_clicked();

    void Barra_ranfo(int range);

    void Barra_de_progreso(int p);


private:
    Ui::Backup_distribuido *ui;
    int Servidor, Cliente_origen, Cliente_destino;
    //creamos las variables que haran de cliente y servidor
    Server* servidor;
    int puerto;
    QString ip;
    int origen;
    SocketTest* cliente;
    int directorio;
    int clientes_enviar;
    int t; //tamaño actual de la barra de progreso
};

#endif // BACKUP_DISTRIBUIDO_H
