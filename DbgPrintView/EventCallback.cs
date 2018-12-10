using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EventLogger;

namespace DbgPrintView
{
    public class EventLoggerArgs : EventArgs
    {
        public Guid ProviderId { get; set; }
    }

    class EventCallback : IEventCallback
    {

        // Event callback for EventLogger callback.
        public event EventHandler<EventLoggerArgs> EventLoggerCallback;

        public EventCallback()
        {
        }

        ~EventCallback()
        {
        }

        #region EventCallback interface
        /// <summary>
        /// OnEventLoggerCallback() gets called when BioSensorEvent occurs.
        /// </summary>
        public void OnEventLoggerCallback(EventLoggerData eventData)
        {
            if (EventLoggerCallback != null)
            {
                EventLoggerCallback(this, new EventLoggerArgs { ProviderId = eventData.ProviderId });
            }

        }

        #endregion
    }
}
