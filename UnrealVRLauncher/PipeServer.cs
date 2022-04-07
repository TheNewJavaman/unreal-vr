using System;
using System.IO;
using System.IO.Pipes;
using System.Threading;

namespace UnrealVR
{
    public class PipeServer
    {
        private NamedPipeServerStream server;
        private PipeStream stream;

        public void Start()
        {
            var thread = new Thread(ServerThread);
            thread.Start();
        }

        private void ServerThread()
        {
            server = new NamedPipeServerStream("UnrealVR", PipeDirection.InOut, 1);
            server.WaitForConnection();
            stream = new PipeStream(server);
            while (server.IsConnected)
                stream.HandleCommand();
        }

        public bool IsConnected { get { return server != null && server.IsConnected; } }

        public void Stop()
        {
            server.Close();
        }
        
        public void SendSettingChange(Setting setting, float value)
        {
            var valueBuffer = BitConverter.GetBytes(value);
            var bufferLength = 1 + valueBuffer.Length;
            var buffer = new byte[bufferLength];
            buffer[0] = (byte) setting;
            valueBuffer.CopyTo(buffer, 1);
            stream.SendCommand(PipeCommand.SettingChange, buffer);
        }
    }

    class PipeStream
    {
        private readonly Stream stream;

        public PipeStream(Stream stream)
        {
            this.stream = stream;
        }

        public void HandleCommand()
        {
            switch ((PipeCommand) stream.ReadByte())
            {
                default:
                    break;
            }
        }

        public void SendCommand(PipeCommand command, byte[] buffer)
        {
            stream.WriteByte((byte) command);
            stream.Write(buffer);
        }
    }

    enum PipeCommand : int
    {
        SettingChange = 0x00
    }

    public enum Setting : int
    {
        CmUnitsScale = 0x00
    }
}
