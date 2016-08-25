using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace ClientDiProva
{

    class Message
    {
        public const string MSG_SEP = "$";
        public const string MSG_SEP_ESC = "&#36";
        public const string MSG_INVALID = "invalid";
        public const string END_MSG = "END_MSG";
        public const int MSG_NUM = 19;
        public string[] messageNames = new string[] {
	        /*  0*/ "USER_REG_REQ",
	        /*  1*/ "USER_REG_REPLY",
	        /*  2*/ "UPDATE_START_REQ",
	        /*  3*/ "UPDATE_START_REPLY",
	        /*  4*/ "ADD_NEW_FILE",
	        /*  5*/ "UPDATE_FILE",
	        /*  6*/ "REMOVE_FILE",
	        /*  7*/ "FILE_ACK",
	        /*  8*/ "UPDATE_STOP_REQ",
	        /*  9*/ "UPDATE_STOP_REPLY",
	        /* 10*/ "GET_VERSIONS_REQ",
	        /* 11*/ "GET_VERSIONS_REPLY",
	        /* 12*/ "GET_LAST_VERSION_REQ",
	        /* 13*/ "GET_LAST_VERSION_REPLY",
	        /* 14*/ "RESTORE_VER_REQ",
	        /* 15*/ "RESTORE_VER_REPLY",
	        /* 16*/ "RESTORE_FILE",
	        /* 17*/ "RESTORE_FILE_ACK",
	        /* 18*/ "RESTORE_STOP",
	        /* 19*/ "PING_REQ",
	        /* 20*/ "PING_REPLY"
        };
        private int fId = 0;
        private string type = "";
        private List<string> items = new List<string>();

        //
        public string getType()
        {
            return this.type;
        }

        public void setType(int type)
        {
            this.type = messageNames[type];
        }

        public void addItem(string item)
        {
            this.items.Add(item);
        }

        public List<string> getItems()
        {
            return this.items;
        }

        public bool Parse(string mex)
        {
            char[] delimiterChars = { '$' };
            string[] words = mex.Split(delimiterChars);
            //Debug.WriteLine("{0} words in text:", words.Length);
            //foreach (string s in words)
            //{
            //    Debug.WriteLine(s);
            //}
            //controllo se è valido
            bool found = false;
            foreach (string i in messageNames)
            {
                if (i.CompareTo(words[0]) == 0)
                {
                    found = true;
                    this.type = i;
                    break;
                }
            }

            if (found == false) return false;

            foreach (string j in words)
            {
                if (j.CompareTo(END_MSG) != 0 && j.CompareTo(this.type)!=0)
                {
                    this.items.Add(j);
                }
            }

            //stampa di verifica
            Debug.WriteLine("Rcevuto messaggio di tipo: "+ this.type + "\nItems: ");
            foreach (string g in this.items) Debug.WriteLine(g);
            Debug.WriteLine("\n\n");
            return true;
        }

        public string ToSend()
        {
            string mex = this.type + MSG_SEP;
            foreach(string i in this.items){
                mex = mex + i + MSG_SEP;
            }
            mex = mex + END_MSG + "\n";
            return mex;
        }
    }
}
