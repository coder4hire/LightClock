package com.lightclockcontrol.gss;

import android.os.Parcel;
import android.os.Parcelable;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import com.lightclockcontrol.gss.dummy.DummyContent.DummyItem;

import java.sql.Time;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;

/**
 * {@link RecyclerView.Adapter} that can display a {@link DummyItem} and makes a call to the
 * specified {@link OnListFragmentInteractionListener}.
 * TODO: Replace the implementation with code for your data type.
 */
public class ScheduleViewAdapter extends RecyclerView.Adapter<ScheduleViewAdapter.ViewHolder> {

    private final ScheduleItem[] mValues;
    private final OnListFragmentInteractionListener mListener;
    public ScheduleViewAdapter(ScheduleItem[] items, OnListFragmentInteractionListener listener) {
        mValues = items;
        mListener = listener;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.fragment_scheduleitem, parent, false);
        return new ViewHolder(view);
    }

    @Override
    public void onBindViewHolder(final ViewHolder holder, int position) {
        holder.mItem = mValues[position];
        holder.chkEnabled.setChecked(holder.mItem.isEnabled);
        Calendar cal = Calendar.getInstance(TimeZone.getTimeZone("UTC"));
        cal.setTime(holder.mItem.execTime);
        holder.lblDescription.setText(String.format("%02d:%02d (",cal.get(Calendar.HOUR_OF_DAY),cal.get(Calendar.MINUTE))
                +holder.mItem.effectType.toString()+")");

        holder.lblDescription.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (null != mListener) {
                    // Notify the active callbacks interface (the activity, if the
                    // fragment is attached to one) that an item has been selected.
                    mListener.onListFragmentInteraction(holder.mItem);
                }
            }
        });
        holder.chkEnabled.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                holder.mItem.isEnabled = holder.chkEnabled.isChecked();
                mListener.onCheckedStateChanged(holder.mItem);
            }
        });
    }

    public boolean UpdateScheduleItem(ScheduleItem item)
    {
        for(int i=0;i<mValues.length;i++)
        {
            if(mValues[i].id == item.id)
            {
                mValues[i]=item;
                notifyDataSetChanged();
                return true;
            }
        }
        return false;
    }

    @Override
    public int getItemCount() {
        return mValues.length;
    }

    public class ViewHolder extends RecyclerView.ViewHolder {
        public final View mView;
        public final CheckBox chkEnabled;
        public final TextView lblDescription;
        public ScheduleItem mItem;

        public ViewHolder(View view) {
            super(view);
            mView = view;
            chkEnabled = (CheckBox) view.findViewById(R.id.chkEnabled);
            lblDescription = (TextView) view.findViewById(R.id.lblDescription);
//            spEffectType = (Spinner) view.findViewById(R.id.effectType);
//            spEffectType.setAdapter(spinnerAdapter);
        }

        @Override
        public String toString() {
            return super.toString() + " '" + lblDescription.getText() + "'";
        }
    }

    public static class ScheduleItem implements Parcelable {
        public final byte id;
        public boolean isEnabled;
        public Date execTime = new Date(0);
        public EffectType effectType = EffectType.None;
        public byte folderID;
        public byte songID;
        public int lightEnabledTime=600;
        public int soundEnabledTime=120;
        public byte dayOfWeekMask = 0;

        public ScheduleItem(byte id) {
            this.id = id;
        }

        protected ScheduleItem(Parcel in) {
            id = in.readByte();
            isEnabled = in.readByte() != 0;
            execTime = new Time(in.readLong());
            effectType = EffectType.fromValue(in.readInt());
            folderID = in.readByte();
            songID = in.readByte();
            lightEnabledTime = in.readInt();
            soundEnabledTime = in.readInt();
            dayOfWeekMask = in.readByte();
        }

        public static final Creator<ScheduleItem> CREATOR = new Creator<ScheduleItem>() {
            @Override
            public ScheduleItem createFromParcel(Parcel in) {
                return new ScheduleItem(in);
            }

            @Override
            public ScheduleItem[] newArray(int size) {
                return new ScheduleItem[size];
            }
        };

        @Override
        public String toString() {
            return Integer.toString(id);
        }

        @Override
        public int describeContents() {
            return 0;
        }

        @Override
        public void writeToParcel(Parcel dest, int flags) {
            dest.writeByte(id);
            dest.writeByte(isEnabled ? (byte)1 : (byte)0);
            dest.writeLong(execTime.getTime());
            dest.writeInt(effectType.getValue());
            dest.writeByte(folderID);
            dest.writeByte(songID);
            dest.writeInt(lightEnabledTime);
            dest.writeInt(soundEnabledTime);
            dest.writeByte(dayOfWeekMask);
        }
    }

    public interface OnListFragmentInteractionListener {
        // TODO: Update argument type and name
        void onListFragmentInteraction(ScheduleViewAdapter.ScheduleItem item);
        void onCheckedStateChanged(ScheduleViewAdapter.ScheduleItem item);
    }
}
