using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace DbgPrintView
{
    public class MessageCommand : ICommand
    {
        public void Execute(object parameter)
        {
            string msg;

            if (parameter == null)
                msg = "Button Clicked!";
            else
                msg = parameter.ToString();

            MessageBox.Show(msg);
        }

        public bool CanExecute(object parameter)
        {
            return true;
        }

        public event EventHandler CanExecuteChanged;
    }

    public class MyDataContext
    {
        ICommand _messageCommand = new MessageCommand();

        public ICommand MessageCommand
        {
            get { return _messageCommand; }
        }
    }
}
