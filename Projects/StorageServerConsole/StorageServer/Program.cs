using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StorageServer
{
    class Program
    {
        static void Main(string[] args)
        {
            StorageServerCore core = null;
            try
            {
                core = new StorageServerCore();
            }
            catch (Exception e)
            {
                Console.WriteLine("**************************************");
                Console.WriteLine("**************************************");
                Console.WriteLine("**  ");
                Console.WriteLine("**  " + e.Message);
                Console.WriteLine("**  ");
                Console.WriteLine("**************************************");
                Console.WriteLine("**************************************");

                if (core != null)
                {
                    core.Dispose();
                }
                return;
            }

            Console.WriteLine("Server started! For more information see log file");

            bool exit = false;
            while (!exit)
            {
                Console.WriteLine("Type \"exit\" to terminate: ");
                string line = Console.ReadLine();
                if (line.ToUpper().Equals("EXIT"))
                    exit = true;
            }

            if (core != null)
            {
                core.Dispose();
            }

            Console.WriteLine("Server stopped.");
            return;
        }
    }
}
