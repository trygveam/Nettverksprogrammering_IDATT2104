import java.io.*;
import java.net.*;

class SocketServer
{
    public static void main(String[] args) throws IOException
    {
        ServerSocket ss = new ServerSocket(1250);

        while (true)
        {
            Socket s = null;

            try
            {
                s = ss.accept();

                System.out.println("A new client is connected : " + s);
                DataInputStream dis = new DataInputStream(s.getInputStream());
                DataOutputStream dos = new DataOutputStream(s.getOutputStream());
                System.out.println("Assigning new thread for this client");
                Thread t = new ClientHandler(s, dis, dos);
                t.start();

            }
            catch (Exception e){
                s.close();
                e.printStackTrace();
            }
        }
    }
}