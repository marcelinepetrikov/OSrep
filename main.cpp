#include <QApplication>
#include <QLabel>
#include <Ice/Ice.h>
#include </home/saps/demo.h>
using namespace std;
using namespace Demo;

int status = 0;
Ice::CommunicatorPtr ic;
int number = 0;
char buff[32];

class OperatorI : public Operator {
private:
    bool isPrimary;
public:
    virtual bool check(const Ice::Current&);
    virtual bool isprimary(const Ice::Current&);
    virtual void setprimary(const Ice::Current&);
    virtual void sendpackage(const Ice::Current&);
    virtual void log(const Ice::Current&);

    QString package;
};

bool OperatorI::check(const Ice::Current&){
    bool result = false;
    try {
        ic = Ice::initialize();

        for(int i=0;i<number;i++) {
            sprintf(buff, "Operator%d:default -p 10000", i);
            Ice::ObjectPrx base = ic->stringToProxy(buff);
            OperatorPrx op = OperatorPrx::checkedCast(base);
            if (!op)
                throw "Invalid proxy";
            else {
                result = op->isprimary();
                if(result)
                    break;
            }
        }
    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();
    return result;
}
bool OperatorI::isprimary(const Ice::Current&){
    return isPrimary;
}
void OperatorI::setprimary(const Ice::Current&){
    if(!this->check(::Ice::Current()))
        isPrimary=true;
}
void OperatorI::sendpackage(const Ice::Current&){
    if(this->isprimary(::Ice::Current()))
        try {
            for(int i = 0;i<number;i++){
                ic = Ice::initialize();
                sprintf(buff, "Operator%d:default -p 10000", i);
                Ice::ObjectPrx base = ic->stringToProxy(buff);
                OperatorPrx op = OperatorPrx::checkedCast(base);
                if (!op)
                    throw "Invalid proxy";
                //op->package+=this->package;
            }
        } catch (const Ice::Exception& ex) {
            cerr << ex << endl;
            status = 1;
        } catch (const char* msg) {
            cerr << msg << endl;
            status = 1;
        }
        if (ic)
            ic->destroy();
}

void OperatorI::log(const Ice::Current&){

}

int main(int argc, char *argv[])
{
    try {
        ic = Ice::initialize(argc, argv);

        for(int i=number;;i++) {
            sprintf(buff, "Operator%d:default -p 10000", i);
            Ice::ObjectPrx base = ic->stringToProxy(buff);
            OperatorPrx op = OperatorPrx::checkedCast(base);
            if (!op){
                number=i;
                break;
            } else {
                continue;
            }
        }
    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();

    try {
        ic = Ice::initialize(argc, argv);
        Ice::ObjectAdapterPtr adapter = ic->createObjectAdapterWithEndpoints("OperatorAdapter", "default -p 10000");
        Ice::ObjectPtr object = new OperatorI;
        sprintf(buff, "Operator%d", number);
        adapter->add(object, ic->stringToIdentity(buff));
        adapter->activate();
        ic->waitForShutdown();
    } catch (const Ice::Exception& e) {
        cerr << e << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic) {
        try {
            ic->destroy();
        } catch (const Ice::Exception& e) {
            cerr << e << endl;
            status = 1;
        }
    }

    QApplication a(argc, argv);

    QLabel l(buff);
    l.setGeometry(0,0,100,100);
    l.setAlignment(Qt::AlignCenter);
    l.show();
    return a.exec();

    return status;
}
