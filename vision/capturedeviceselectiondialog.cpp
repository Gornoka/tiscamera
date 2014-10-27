#include "capturedeviceselectiondialog.h"
#include "ui_capturedeviceselectiondialog.h"

#include <unistd.h>

DeviceInfoSelectionDialog::DeviceInfoSelectionDialog (QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceInfoSelectionDialog),
    device_watch_dog(tcam::getDeviceIndex())
{
    ui->setupUi(this);

    run_thread = true;

    work_thread = std::thread(&DeviceInfoSelectionDialog::update_list, this);
}


DeviceInfoSelectionDialog::~DeviceInfoSelectionDialog ()
{
    run_thread = false;
    work_thread.join();
    delete ui;
}


void DeviceInfoSelectionDialog::on_buttonBox_accepted ()
{
    auto selected_item = this->ui->device_table->selectedItems();

    if (selected_item.empty())
    {
        //return DeviceInfo();
        return;
    }

    QString serial = selected_item[1]->text();

    auto f = [&serial] (const tcam::DeviceInfo& dev)
    {
        return serial.toStdString().compare(dev.getSerial()) == 0;
    };

    auto d = std::find_if(devices.begin(), devices.end(), f);

    if (d == devices.end())
    {
        return;
    }

    //return *d;
    emit device_selected(*d);
    accept();
}


void DeviceInfoSelectionDialog::update_list ()
{

    while (run_thread)
    {
        if (device_watch_dog == nullptr)
            device_watch_dog = tcam::getDeviceIndex();

        devices = device_watch_dog->getDeviceList();
        ui->device_table->clear();
        this->ui->device_table->setRowCount(devices.size());

        bool is_empty = devices.empty();
        if (is_empty)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        unsigned int row = 0;
        for (auto& d : devices)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem( QString(d.getName().c_str()));
            QTableWidgetItem *newSerial = new QTableWidgetItem( QString(d.getSerial().c_str()));
            QTableWidgetItem *newType = new QTableWidgetItem( QString(d.getDeviceTypeAsString().c_str()));

            ui->device_table->setItem(row, 0, newItem);
            ui->device_table->setItem(row, 1, newSerial);
            ui->device_table->setItem(row, 2, newType);
            row++;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


void DeviceInfoSelectionDialog::on_buttonBox_rejected ()
{
    QWidget::close();
}


tcam::DeviceInfo DeviceInfoSelectionDialog::getSelection ()
{
    auto selected_item = this->ui->device_table->selectedItems();

    if (selected_item.empty())
    {
        return DeviceInfo();
    }

    QString serial = selected_item[0]->text();

    auto f = [&serial] (const tcam::DeviceInfo& dev)
    {
        return serial.toStdString().compare(dev.getSerial()) == 0;
    };

    auto d = std::find_if(devices.begin(), devices.end(), f);

    if (d == devices.end())
    {
        return DeviceInfo();
    }

    return *d;
}
