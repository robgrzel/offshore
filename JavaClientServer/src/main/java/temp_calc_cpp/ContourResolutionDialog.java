package temp_calc_cpp;

import java.awt.*;

public class ContourResolutionDialog extends Dialog {
    void resolutionSlider_ScrollAbsolute(Event event) {

        //{{CONNECTION
        // Set the text for TextField...
        panelCountField.setText(String.valueOf(resolutionSlider.getValue()));
        //}}
    }

    void OKbutton_Clicked(Event event)
    {
        Client.dialogDismissed(this);
        hide();
    }

    void ApplyButton_Clicked(Event event)
    {
        Client.dialogDismissed(this);
    }


    public ContourResolutionDialog(Frame parent, boolean modal) {

        super(parent, modal);

        //{{INIT_CONTROLS
        setLayout(null);
        addNotify();
        resize(insets().left + insets().right + 172,insets().top + insets().bottom + 271);
        OKbutton = new java.awt.Button("OK");
        OKbutton.reshape(insets().left + 108,insets().top + 132,48,29);
        add(OKbutton);
        ApplyButton = new java.awt.Button("Apply");
        ApplyButton.reshape(insets().left + 108,insets().top + 84,48,29);
        add(ApplyButton);
        panelCountField = new java.awt.TextField();
        panelCountField.setEditable(false);
        panelCountField.reshape(insets().left + 28,insets().top + 192,52,35);
        add(panelCountField);
        label2 = new java.awt.Label("# of panels");
        label2.reshape(insets().left + 12,insets().top + 228,92,20);
        add(label2);
        resolutionSlider = new java.awt.Scrollbar(Scrollbar.VERTICAL,0,0,5,200);
        resolutionSlider.reshape(insets().left + 48,insets().top + 16,20,160);
        add(resolutionSlider);
        setTitle("Contour Resolution");
        //}}
        Client = null;
    }

    public ContourResolutionDialog(Frame parent, ColorContourPlot client, boolean modal)
    {
        this(parent, modal);
        Client = client;
    }

    public synchronized void show() {
        Rectangle bounds = getParent().bounds();
        Rectangle abounds = bounds();

        move(bounds.x + (bounds.width - abounds.width)/ 2,
                bounds.y + (bounds.height - abounds.height)/2);

        super.show();
    }

    public boolean handleEvent(Event event) {
        if(event.id == Event.WINDOW_DESTROY) {
            hide();
            return true;
        }
        if (event.target == OKbutton && event.id == Event.ACTION_EVENT) {
            OKbutton_Clicked(event);
            return true;
        }
        if (event.target == ApplyButton && event.id == Event.ACTION_EVENT) {
            ApplyButton_Clicked(event);
            return true;
        }
        if (event.target == resolutionSlider) {
            resolutionSlider_ScrollAbsolute(event);
            return true;
        }
        return super.handleEvent(event);
    }

    //{{DECLARE_CONTROLS
    java.awt.Button OKbutton;
    java.awt.Button ApplyButton;
    java.awt.TextField panelCountField;
    java.awt.Label label2;
    java.awt.Scrollbar resolutionSlider;
    //}}



    public void setResolution(int panelCount)
    {
        xPanelCount = panelCount;
        yPanelCount = panelCount;
        panelCountField.setText(String.valueOf(panelCount));
        resolutionSlider.setValue(panelCount);
    }

    public void captureResolution()
    {
        xPanelCount = (new Integer(panelCountField.getText())).intValue();
        yPanelCount = (new Integer(panelCountField.getText())).intValue();
    }

    public int  getResolution()
    {
        captureResolution();
        return xPanelCount;
    }


    int xPanelCount;
    int yPanelCount;

    ColorContourPlot Client;
}