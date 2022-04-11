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
            server = new NamedPipeServerStream("UnrealVR", PipeDirection.Out, 1);
            server.WaitForConnection();
            stream = new PipeStream(server);
        }

        public bool IsConnected { get { return server != null && server.IsConnected; } }

        public void Stop()
        {
            if (server != null)
            {
                if (server.IsConnected)
                    server.Disconnect();
                server.Dispose();
            }
        }
        
        public void SendSettingChange(Setting setting, float value)
        {
            var valueBuffer = BitConverter.GetBytes(value);
            var bufferLength = 1 + valueBuffer.Length;
            var buffer = new byte[bufferLength];
            buffer[0] = (byte) setting;
            valueBuffer.CopyTo(buffer, 1);
            stream.SendCommand(buffer);
        }
    }

    class PipeStream
    {
        private readonly Stream stream;

        public PipeStream(Stream stream)
        {
            this.stream = stream;
        }

        public void SendCommand(byte[] buffer)
        {
            stream.Write(buffer);
            stream.Flush();
        }
    }

    public enum Setting : int
    {
        CmUnitsScale = 0x00,
        FOVScale = 0x01
    }
}
