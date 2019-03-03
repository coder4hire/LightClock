package com.lightclockcontrol.gss;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.SeekBar;

import top.defaults.colorpicker.ColorObserver;
import top.defaults.colorpicker.ColorPickerView;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link ManualFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class ManualFragment extends Fragment {
    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;


    public ManualFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment ManualFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static ManualFragment newInstance(String param1, String param2) {
        ManualFragment fragment = new ManualFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_manual, container, false);

        final ColorPickerView colorPickerView = view.findViewById(R.id.colorPickerView);
        final SeekBar seekWhite = view.findViewById(R.id.seekWhite);
        final CheckBox chkEnableLEDs = view.findViewById(R.id.chkEnableLEDs);

        colorPickerView.subscribe(new ColorObserver() {
            @Override
            public void onColor(int color, boolean fromUser, boolean shouldPropagate) {
                if(chkEnableLEDs.isChecked())
                {
                    BTInterface.GetInstance().SendSetManualColorPacket(color,seekWhite.getProgress());
                }
            }
        });

        seekWhite.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(chkEnableLEDs.isChecked())
                {
                    BTInterface.GetInstance().SendSetManualColorPacket(colorPickerView.getColor(),progress);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        chkEnableLEDs.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(chkEnableLEDs.isChecked())
                {
                    BTInterface.GetInstance().SendSetManualColorPacket(colorPickerView.getColor(),seekWhite.getProgress());
                }
                else
                {
                    BTInterface.GetInstance().SendSetManualColorPacket(0,0);
                }
            }
        });

        return view;
    }

}
