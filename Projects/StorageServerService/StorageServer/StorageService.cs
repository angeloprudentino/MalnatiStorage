using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace StorageServer
{
    public partial class StorageService : ServiceBase
    {
        private StorageServerCore core = null;

        public StorageService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            try
            {
                if (this.core != null)
                    this.core.Dispose(); 
                
                this.core = new StorageServerCore();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.Message);
            }
        }

        protected override void OnStop()
        {
            try
            {
                if (this.core != null)
                    this.core.Dispose();

                this.core = null;
            }
            catch (Exception e)
            {
                Debug.WriteLine(e.Message);
            }        
        }
    }
}
