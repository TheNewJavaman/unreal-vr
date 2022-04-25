using System;
using System.IO;
using System.IO.Pipes;
using System.Threading.Tasks;

namespace UnrealVR
{
    public class PipeServer
    {
        private NamedPipeServerStream server;
        private PipeStream stream;

        public PipeServer()
        {
            server = new NamedPipeServerStream("UnrealVR", PipeDirection.Out, 1);
            stream = new PipeStream(server);
        }

        public async Task WaitForConnection()
        {
            if (IsConnected) return;
            await server.WaitForConnectionAsync();
        }

        public bool IsConnected { get { return server.IsConnected; } }

        public void Disconnect()
        {
            if (IsConnected) server.Disconnect();
        }

        public async Task SendSettingChangeAsync(Setting setting, float value)
        {
            var valueBuffer = BitConverter.GetBytes(value);
            var bufferLength = 1 + valueBuffer.Length;
            var buffer = new byte[bufferLength];
            buffer[0] = (byte) setting;
            valueBuffer.CopyTo(buffer, 1);
            await stream.SendCommandAsync(buffer);
        }
    }

    class PipeStream
    {
        private readonly Stream stream;

        public PipeStream(Stream stream)
        {
            this.stream = stream;
        }

        public async Task SendCommandAsync(byte[] buffer)
        {
            await stream.WriteAsync(buffer);
            await stream.FlushAsync();
        }
    }

    public enum Setting : int
    {
        CmUnitsScale = 0x00
    }
}
