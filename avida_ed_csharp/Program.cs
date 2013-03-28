using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

using avida_wrapper;

namespace avida_ed_csharp
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            avida_wrapper.Avida_Wrapper.Initialize();
//            Avida.Initialize();
//            avida_wrapper.Avida::Initialize();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new AvidaEdMainWindow());
        }
    }
}
