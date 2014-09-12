/*
 * log-inspector.cc
 * Copyright 2014 William Pitcock
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions, and the following disclaimer in the documentation
 *    provided with the distribution.
 *
 * This software is provided "as is" and without any warranty, express or
 * implied. In no event shall the authors be liable for any damages arising from
 * the use of this software.
 */

#include <QtGui>
#include <QtWidgets>
#include <QAbstractTableModel>

#include <libaudcore/i18n.h>
#include <libaudcore/preferences.h>
#include <libaudcore/runtime.h>

#include <libaudqt/libaudqt.h>

#include "log-inspector.h"
#include "log-inspector.moc"

namespace audqt {

const int LOGENTRY_MAX = 1000;

enum LogEntryColumn {
    Level,
    File,
    Line,
    Function,
    Message,
    Count
};

static Index<SmartPtr<LogEntry>> entries;

/* log entry model */
LogEntryModel::LogEntryModel (QObject * parent) : QAbstractListModel (parent)
{
}

LogEntryModel::~LogEntryModel ()
{
}

int LogEntryModel::rowCount (const QModelIndex & parent) const
{
    return entries.len ();
}

int LogEntryModel::columnCount (const QModelIndex & parent) const
{
    return LogEntryColumn::Count;
}

QVariant LogEntryModel::data (const QModelIndex & index, int role) const
{
    auto & e = entries [index.row ()];

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column ())
        {
            case LogEntryColumn::Level: return QString (audlog::get_level_name (e->level));
            case LogEntryColumn::File: return QString (e->filename);
            case LogEntryColumn::Line: return e->line;
            case LogEntryColumn::Function: return QString (e->function);
            case LogEntryColumn::Message: return QString (e->message);
        }
        break;
    }

    return QVariant ();
}

QVariant LogEntryModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case LogEntryColumn::Level: return QString (translate_str ("Level"));
            case LogEntryColumn::File: return QString (translate_str ("Filename"));
            case LogEntryColumn::Line: return QString (translate_str ("Line"));
            case LogEntryColumn::Function: return QString (translate_str ("Function"));
            case LogEntryColumn::Message: return QString (translate_str ("Message"));
        }
    }

    return QVariant ();
}

bool LogEntryModel::insertRows (int row, int count, const QModelIndex & parent)
{
    int last = row + count - 1;
    beginInsertRows (parent, row, last);
    endInsertRows ();
    return true;
}

bool LogEntryModel::removeRows (int row, int count, const QModelIndex & parent)
{
    int last = row + count - 1;
    beginRemoveRows (parent, row, last);
    endRemoveRows ();
    return true;
}

void LogEntryModel::updateRows (int row, int count)
{
    int bottom = row + count - 1;
    auto topLeft = createIndex (row, 0);
    auto bottomRight = createIndex (bottom, columnCount () - 1);
    emit dataChanged (topLeft, bottomRight);
}

void LogEntryModel::updateRow (int row)
{
    updateRows (row, 1);
}

/* log entry inspector */
class LogEntryInspector : public QDialog
{
public:
    LogEntryInspector (QWidget * parent = nullptr);
    ~LogEntryInspector ();

    audlog::Level m_level;

    void pop ();
    void push ();

private:
    QVBoxLayout m_layout;
    LogEntryModel * m_model;
    QTreeView * m_view;

    void setLogLevel (audlog::Level level);
};

LogEntryInspector::LogEntryInspector (QWidget * parent) :
    QDialog (parent)
{
    setWindowTitle (translate_str ("Log Inspector"));
    setLayout (& m_layout);

    m_model = new LogEntryModel (this);
    m_view = new QTreeView (this);
    m_view->setModel (m_model);

    setLogLevel (audlog::Info);

    m_layout.addWidget (m_view);

    resize (800, 350);
}

LogEntryInspector::~LogEntryInspector ()
{
}

static LogEntryInspector * m_inspector = nullptr;

static void log_handler (audlog::Level level, const char * file, int line, const char * func, const char * message)
{
    LogEntry * l = new LogEntry;

    l->level = level;
    l->filename = file;
    l->line = line;
    l->function = func;

    l->message = strdup(message);
    l->message[strlen (l->message) - 1] = 0;

    entries.append (SmartPtr<LogEntry> (l));

    if (entries.len () > LOGENTRY_MAX)
    {
        m_inspector->pop ();
        entries.erase (0, 1);
    }

    m_inspector->push ();
}

void LogEntryInspector::setLogLevel (audlog::Level level)
{
    m_level = level;

    audlog::unsubscribe (log_handler);
    audlog::subscribe (log_handler, level);
}

void LogEntryInspector::pop ()
{
    m_model->removeRows (0, 1);
}

void LogEntryInspector::push ()
{
    m_model->insertRows (entries.len (), 1);
#ifdef XXX_NOTYET
    auto index = m_model->index (entries.len () - 1);
    m_view->scrollTo (index);
#endif
}

EXPORT void log_inspector_show ()
{
    if (! m_inspector)
        m_inspector = new LogEntryInspector;

    window_bring_to_front (m_inspector);
}

EXPORT void log_inspector_hide ()
{
    if (! m_inspector)
        return;

    m_inspector->hide ();
}

}