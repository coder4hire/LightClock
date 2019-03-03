package com.lightclockcontrol.gss;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.support.design.internal.ParcelableSparseArray;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.lang.reflect.Array;
import java.util.ArrayList;


/**
 * A simple {@link Fragment} subclass.
 * Use the {@link AlarmsFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class AlarmsFragment extends Fragment {
    RecyclerView scheduleList;
    private RecyclerView.LayoutManager layoutManager;
    private ScheduleViewAdapter adapter;
    final int ALARMS_NUM = 10;

    private ScheduleViewAdapter.ScheduleItem[] scheduleItems = new ScheduleViewAdapter.ScheduleItem[ALARMS_NUM];

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    //   private OnFragmentInteractionListener mListener;

    public AlarmsFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment AlarmsFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static AlarmsFragment newInstance(String param1, String param2) {
        AlarmsFragment fragment = new AlarmsFragment();
        Bundle args = new Bundle();
//        args.putString(ARG_PARAM1, param1);
//        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //TODO: replace with request to alarm clock retrieving actual schedule
        for (byte i = 0; i < ALARMS_NUM; i++) {
            scheduleItems[i]=new ScheduleViewAdapter.ScheduleItem(i);
        }

        adapter = new ScheduleViewAdapter(scheduleItems, new ScheduleViewAdapter.OnListFragmentInteractionListener() {
            @Override
            public void onListFragmentInteraction(ScheduleViewAdapter.ScheduleItem item) {
                Intent i = new Intent(getActivity().getBaseContext(), ScheduleItemEditor.class);
                i.putExtra("EditingItem", item);
                startActivityForResult(i,0);
            }

            @Override
            public void onCheckedStateChanged(ScheduleViewAdapter.ScheduleItem item) {
                BTInterface.GetInstance().SendEnableScheduleItem(item.id,item.isEnabled);
            }
        });

        //        if (getArguments() != null) {
//            mParam1 = getArguments().getString(ARG_PARAM1);
//            mParam2 = getArguments().getString(ARG_PARAM2);
//        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_alarms, container, false);

        scheduleList = (RecyclerView) view.findViewById(R.id.scheduleList);
        scheduleList.setHasFixedSize(true);

        layoutManager = new LinearLayoutManager(this.getContext());
        scheduleList.setLayoutManager(layoutManager);

        scheduleList.setAdapter(adapter);

        return view;
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode==1)
        {
            Parcelable[] parcel = data.getExtras().getParcelableArray("ResultItem");
            if(parcel!=null) {
                UpdateSchedule((ScheduleViewAdapter.ScheduleItem[]) parcel);
            }
        }
    }

    public void UpdateSchedule(ScheduleViewAdapter.ScheduleItem[] newItems)
    {
        for(ScheduleViewAdapter.ScheduleItem item : newItems) {
            adapter.UpdateScheduleItem(item);
        }
    }

    @Override
    public void onResume()
    {
        super.onResume();
        BTInterface.GetInstance().SendGetSchedule();
    }
}