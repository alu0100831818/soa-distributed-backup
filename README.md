# Backup distribuido

Se ha desarrollado un software que pretende realizar un backup distribuido.
Por un lado esta el cliente quien podrá enviar o recibir datos, por otro el servidor que hara de intermedio entre
el cliente oorigen de los datos y el/los cliente/s destino. 

### Funcionamiento

Para empezar hay que tener en cuenta lo siguiente:
 1. El servidor ha de iniciarse antes que los clientes, estara escuchando en una Ip y un Puerto, internamente se ha diseñado
    para que escuche en un puerto a cualquier ip que desee comunicarse con el.
 2.  Se puede inciar tanto un cliente destino como un cliente origen.
 3. Se ha de indicar en cada apartado correctamente los datos de las ip y puerto, no se contempan errores en cuanto 
    a que los datos esten mal introducidos, se le comunica al usuario que no se ha podido inciar sesion, pero no se especifica
    el por que. 
    a- Servidor:
	![Ventana](/../master/images/servidor.png){:height="100px" width="100px"}
    b- cliente Origen:
	![Ventana](/../master/images/cliO.png){:height="100px" width="100px"}
    c- cliente Destino:
	![Ventana](/../master/images/cliD.png){:height="100px" width="100px"}


## Protocolo

Se trata de un protocolo de texto basado en TCP. 
Transporta cadenas reconocibles por elservidor y los clientes para comunicar las acciones que estos desean realizar.

Las cadenas reconocibles tienen un formato muy limitado, se incorporan delimitadores como espacios o saltos de linea 
entre la informacion que se quiere enviar. Dependiendo de la accion a comunicar el tamaño de los mensajes puede variar
de 4 bytes en adelante. 


