package com.lightclockcontrol.gss;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.CheckBox;
import android.widget.TextView;

import com.lightclockcontrol.gss.dummy.DummyContent;
import com.lightclockcontrol.gss.dummy.DummyContent.DummyItem;

import java.sql.Time;
import java.util.Date;
import java.util.List;

/**
 * {@link RecyclerView.Adapter} that can display a {@link DummyItem} and makes a call to the
 * specified {@link OnListFragmentInteractionListener}.
 * TODO: Replace the implementation with code for your data type.
 */
public class ScheduleViewAdapter extends RecyclerView.Adapter<ScheduleViewAdapter.ViewHolder> {

    private final List<ScheduleItem> mValues;
    private final OnListFragmentInteractionListener mListener;
    ArrayAdapter<String> spinnerAdapter;

    public ScheduleViewAdapter(List<ScheduleItem> items, OnListFragmentInteractionListener listener) {
        mValues = items;
        mListener = listener;
        spinnerAdapter= new ArrayAdapter<String>(App.getContext(), android.R.layout.simple_spinner_item);
        for (EffectType t: EffectType.values()){
            spinnerAdapter.add(t.toString());
        }
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.fragment_scheduleitem, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final ViewHolder holder, int position) {
        holder.mItem = mValues.get(position);
        holder.chkEnabled.setChecked(holder.mItem.isEnabled);
        holder.viewTime.setText(holder.mItem.execTime.toString());
        holder.spEffectType.setSelection(holder.mItem.effectType.getValue());

        holder.mView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (null != mListener) {
                    // Notify the active callbacks interface (the activity, if the
                    // fragment is attached to one) that an item has been selected.
                    mListener.onListFragmentInteraction(holder.mItem);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return mValues.size();
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public final View mView;
        public final CheckBox chkEnabled;
        public final TextView viewTime;
        public Spinner spEffectType;
        public ScheduleItem mItem;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            chkEnabled = (CheckBox) view.findViewById(R.id.isEnabled);
            viewTime = (TextView) view.findViewById(R.id.execTime);
            spEffectType = (Spinner) view.findViewById(R.id.effectType);
            spEffectType.setAdapter(spinnerAdapter);
        }

        @Override
        public String toString() {
            return super.toString() + " '" + viewTime.getText() + "'";
        }
    }

    public static class ScheduleItem {
        public final int id;
        public boolean isEnabled;
        public Date execTime = new Time(0);
        public EffectType effectType = EffectType.None;

        public ScheduleItem(int id) {
            this.id = id;
        }

        @Override
        public String toString() {
            return Integer.toString(id);
        }
    }

    public interface OnListFragmentInteractionListener {
        // TODO: Update argument type and name
        void onListFragmentInteraction(ScheduleViewAdapter.ScheduleItem item);
    }

}
