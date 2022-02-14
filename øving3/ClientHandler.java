import jdk.jfr.Experimental;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

class ClientHandler extends Thread
{
    final DataInputStream dis;
    final DataOutputStream dos;
    final Socket s;

    public ClientHandler(Socket s, DataInputStream dis, DataOutputStream dos)
    {
        this.s = s;
        this.dis = dis;
        this.dos = dos;
    }

    @Override
    public void run()
    {
        String received;
        String toreturn;
        try{
            dos.writeUTF("Welcome to this Calculator Client System");
            dos.writeUTF("Type in your desired math problem ex '2+2', type 'Exit' to close application.");
        }catch (IOException e){
            e.printStackTrace();
        }

        while (true)
        {
            try {
                received = dis.readUTF();
                if(received.equals("Exit"))
                {
                    System.out.println("Client " + this.s + " sends exit...");
                    System.out.println("Closing this connection.");
                    this.s.close();
                    System.out.println("Connection closed");
                    break;
                }
                int sum = 0;
                String[] parts = received.split("(?=[/*+-])|(?<=[/*+-])");
                switch (parts[1]){
                    case "+":
                        sum = Integer.parseInt(parts[0]) + Integer.parseInt(parts[2]);
                        break;
                    case "-":
                        sum = Integer.parseInt(parts[0]) - Integer.parseInt(parts[2]);
                        break;
                    case "*":
                        sum = Integer.parseInt(parts[0]) * Integer.parseInt(parts[2]);
                        break;
                    case "/":
                        sum  = Integer.parseInt(parts[0]) / Integer.parseInt(parts[2]);
                        break;
                }
                dos.writeUTF("The solution is : " + sum);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        try
        {
            this.dis.close();
            this.dos.close();

        }catch(IOException e){
            e.printStackTrace();
        }
    }
}