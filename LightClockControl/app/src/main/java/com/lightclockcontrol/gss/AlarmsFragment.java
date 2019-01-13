package com.lightclockcontrol.gss;

import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link AlarmsFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link AlarmsFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class AlarmsFragment extends Fragment {
    RecyclerView scheduleList;
    private RecyclerView.LayoutManager layoutManager;
    ScheduleViewAdapter adapter;
    final int ALARMS_NUM = 10;

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
        ArrayList<ScheduleViewAdapter.ScheduleItem> items = new ArrayList<>();
        for (int i = 0; i < ALARMS_NUM; i++) {
            items.add(new ScheduleViewAdapter.ScheduleItem(i));
        }

        adapter = new ScheduleViewAdapter(items, new ScheduleViewAdapter.OnListFragmentInteractionListener() {
            @Override
            public void onListFragmentInteraction(ScheduleViewAdapter.ScheduleItem item) {
            }
        });
        scheduleList.setAdapter(adapter);

        return view;
    }
}