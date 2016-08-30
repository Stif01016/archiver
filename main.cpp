#include <iostream>
#include <gtkmm.h>
#include <gtkmm/application.h>
#include "lzwfin.hpp"
#include <fstream>
#include <gtkmm/entry.h>
#include <cstring>


class MainWindow : public Gtk::Window
{
public:
    MainWindow()
        :m_Button_Compress("COMPRESS"),
         m_Button_Deccompress("DECOMPRESS"),
         m_Box_Top(Gtk::ORIENTATION_VERTICAL),
         m_Box_Button(Gtk::ORIENTATION_VERTICAL,1),
         m_Box_other(Gtk::ORIENTATION_VERTICAL,10),
         m_Box_Edit(Gtk::ORIENTATION_HORIZONTAL,6),
         m_Button_Bar_Clear("Clear",true),
         m_Box_clear_button(Gtk::ORIENTATION_VERTICAL,10)
    {
        set_title("LZW ARCHIEVER");
        add(m_Box_Top);
        set_size_request(450,300);

        m_Entry_input_file.set_max_length(50);
        m_Entry_input_file.select_region(0,m_Entry_input_file.get_text_length());
        m_Entry_output_folder.select_region(0,m_Entry_output_folder.get_text_length());
        m_Box_other.pack_start(m_Entry_input_file);
        m_Box_other.pack_start(m_Entry_output_folder);

        Gtk::Container* infoBarContainer=dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
        if(infoBarContainer)
            infoBarContainer->add(m_Message_Label);
        m_InfoBar.add_button("OK",0);

        m_Box_Edit.pack_start(m_InfoBar,Gtk::PACK_SHRINK);
        m_Box_Top.pack_start(m_Box_Button);
        m_Box_Top.pack_start(m_Box_other);
        m_Box_Top.pack_start(m_Box_Edit);
        m_Box_Top.pack_start(m_Box_clear_button);
        m_Box_Button.pack_start(m_Grid);

        m_refTextBuffer=Gtk::TextBuffer::create();
        m_TextView.set_buffer(m_refTextBuffer);

        m_Box_clear_button.pack_start(m_Button_Bar_Clear);
        m_ScrolledWindow.add(m_TextView);
        m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
        m_Box_Edit.pack_start(m_ScrolledWindow);

        m_Grid_Box.attach(m_Box_Button,0,0,1,1);
        m_Box_Button.property_margin()=5;
        m_Grid_Box.attach(m_Box_other,1,0,1,1);


        m_Box_Button.set_border_width(10);
        m_Box_other.set_border_width(10);
        m_Box_Button.pack_start(m_Grid);
        m_Grid.set_row_homogeneous(true);

        m_Grid.attach(m_Button_Compress,1,0,1,1);
        m_Button_Compress.property_margin()=5;
        m_Button_Compress.signal_clicked().connect(sigc::mem_fun(*this,&MainWindow::click_button_file_compress));

        m_Grid.attach(m_Button_Deccompress,0,0,1,1);
        m_Button_Deccompress.property_margin()=5;
        m_Button_Deccompress.signal_clicked().connect(sigc::mem_fun(*this,&MainWindow::click_button_file_decompress));

        m_InfoBar.signal_response().connect(sigc::mem_fun(*this,&MainWindow::on_infobar_response));
        m_Button_Bar_Clear.signal_clicked().connect(sigc::mem_fun(*this,&MainWindow::on_button_clear));
        m_refTextBuffer->signal_changed().connect(sigc::mem_fun(*this,&MainWindow::on_textbuffer_changed));
        show_all();
        show_all_children();
        m_InfoBar.hide();
        m_Button_Bar_Clear.set_sensitive(false);
    }
    virtual ~MainWindow()
    {
    }
protected:
    void on_button_file_compressed();
    void on_button_file_decompressed();
    void click_button_file_compress();
    void click_button_file_decompress();
    void on_button_clear();
    void on_infobar_response(int response);
    void on_textbuffer_changed();
    bool lzw_file(std::string name);
    std::string choose_foilder();
    std::string output_name(std::string name);
    std::string output_name_decompress(std::string name);
    Gtk::Button m_Button_About;
    Gtk::Button m_Button_Bar_Clear;
    Gtk::Button m_Button_Compress, m_Button_Deccompress;
    Gtk::Grid m_Grid;
    Gtk::Box m_Box_Button;
    Gtk::Separator m_Separator;
    Gtk::Box m_Box_Top;
    Gtk::Box m_Box_other;
    Gtk::Box m_Box_clear_button;
    Gtk::Entry m_Entry_input_file;
    Gtk::Entry m_Entry_output_folder;
    Gtk::Box m_Box_Edit;
    Gtk::Grid m_Grid_Box;
    Gtk::InfoBar m_InfoBar;
    Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer;
    Gtk::Label m_Message_Label;
    Gtk::TextView m_TextView;
    Gtk::ScrolledWindow m_ScrolledWindow;
};

std::string MainWindow::choose_foilder()
{
    Gtk::FileChooserDialog dialog("please choose a folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.add_button("Select", Gtk::RESPONSE_OK);
    int result=dialog.run();
    switch(result){
    case(Gtk::RESPONSE_OK):
        return dialog.get_filename();
    }
}
void MainWindow::on_textbuffer_changed()
{
    m_Button_Bar_Clear.set_sensitive(m_refTextBuffer->size()>0);
}

void MainWindow::on_infobar_response(int response)
{
    m_Message_Label.set_text("");
    m_InfoBar.hide();
}

void MainWindow::on_button_clear()
{
    m_refTextBuffer->set_text("");
    m_Message_Label.set_text("Cleared the text");
    m_InfoBar.set_message_type(Gtk::MESSAGE_INFO);
    m_InfoBar.show();
}

bool MainWindow::lzw_file(std::string name)
{
    if(name.length()<4)
        return false;
    else{
        std::string test_name;
        std::fstream test(test_name);
        for(int counter=name.length()-4; counter<name.length();counter++){
            test_name+=name[counter];
        }
        if(test_name!=".lzw")
            return false;
        else
            return true;
    }
}
void MainWindow::click_button_file_decompress()
{
    int max_code=32767;
    std::string File_input=m_Entry_input_file.get_text();
    std::ifstream input_file(File_input);
    if(!input_file.is_open()){
        on_button_file_decompressed();
    }else{
        if(lzw_file(File_input)==true){
            std::string File_output=m_Entry_output_folder.get_text()+output_name(output_name_decompress(File_input));
            std::istream *in=&std::cin;
            std::ostream *out=&std::cout;
            in=new std::ifstream(File_input);
            out=new std::ofstream(File_output);
            Decompress(*in,*out,max_code);
            delete in;
            delete out;
            m_refTextBuffer->set_text(" file : "+File_input+" decompressed ");
        }else{
            on_button_file_decompressed();

        }
    }
}
void MainWindow::click_button_file_compress()
{
    int max_code=32767;
    std::string File_input=m_Entry_input_file.get_text();
    std::ifstream input_file(File_input);
    std::string File_output=m_Entry_output_folder.get_text()+"/"+output_name(File_input)+".lzw";
    std::ofstream output_file(File_output);
    if(!input_file.is_open()){
        on_button_file_compressed();
    }
    else{
        std::istream *in=&std::cin;
        std::ostream *out=&std::cout;
        in=new std::ifstream(File_input);
        out=new std::ofstream(File_output);
        Compress(*in,*out,max_code);
        delete in;
        delete out;
        m_refTextBuffer->set_text("file : "+File_input+ " compressed");
    }
}

void MainWindow::on_button_file_compressed()
{
    unsigned int max_code=32767;
    Gtk::FileChooserDialog dialog("Please Choose File",Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("OK",Gtk::RESPONSE_OK);

    Glib::RefPtr<Gtk::FileFilter> filter_any=Gtk::FileFilter::create();
    filter_any->set_name("Choose file for compression");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);
    int result=dialog.run();
    switch(result)
        {
        case (Gtk::RESPONSE_OK):
            {
                std::cout<<"OPEN CLICKED"<<std::endl;
                std::string filename=dialog.get_filename();
                std::string outfilename=choose_foilder()+"/"+output_name(filename)+".lzw";
                std::cout<<outfilename<<std::endl;
                std::cout<<output_name(filename)<<std::endl;
                std::istream *in=&std::cin;
                std::ostream *out=&std::cout;
                in=new std::ifstream(filename);
                out=new std::ofstream(outfilename);
                Compress(*in,*out,max_code);
                delete in;
                delete out;
                std::cout<<"FILE SELECTED :  "<<filename<<std::endl;
                m_refTextBuffer->set_text(" file : "+filename+" compressed");
                break;
            }
        case(Gtk::RESPONSE_CANCEL):
            {
                std::cout<<"Cancel clicked"<<std::endl;
                break;
            }
        default:
            {
                std::cout<<"UNEXPECTED BUTTON CLICKED"<<std::endl;
                break;
            }
        }
}

void MainWindow::on_button_file_decompressed()
{
    unsigned int max_code = 32767;
    Gtk::FileChooserDialog dialog("Please choose file",Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("OK",Gtk::RESPONSE_OK);

    Glib::RefPtr<Gtk::FileFilter> arc_filter=Gtk::FileFilter::create();
    arc_filter->set_name("LZW files");
    arc_filter->add_pattern("*.lzw");
    dialog.add_filter(arc_filter);

    int result=dialog.run();
    switch(result)
        {
        case(Gtk::RESPONSE_OK):
            {
                std::cout<<"open clicked"<<std::endl;
                std::string input_file=dialog.get_filename();
                std::cout<<"UNPACKING FILE"<<input_file<<std::endl;
                std::string output_file=choose_foilder()+"/"+output_name_decompress(output_name(input_file));
                std::cout<<output_file<<std::endl;
                std::istream *in=&std::cin;
                std::ostream *out=&std::cout;
                in=new std::ifstream(input_file);
                out=new std::ofstream(output_file);
                Decompress(*in,*out,max_code);
                delete in;
                delete out;
                std::cout<<"UNPACKED FILE"<<output_file<<std::endl;
                m_refTextBuffer->set_text(" file : "+input_file+" decompressed");
            }
        }
}


std::string MainWindow::output_name(std::string name)
{
    std::string out;
    std::string output;
    for(int counter=name.length()-1; counter>=0;counter--){
        out+=name[counter];
        if(name[counter-1]=='/'){
            break;
        }
    }
    for(int counter=out.length()-1;counter>=0;counter--){
        output+=out[counter];
    }
    return output;
}

std::string MainWindow::output_name_decompress(std::string name)
{
    std::string out;
    for(int counter=0;counter<name.length()-4;counter++){
        out+=name[counter];
    }
    return out;
}
int main(int argc, char * argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc,argv,"org.gtkmm.example");
    MainWindow window;
    return  app->run(window);
}
