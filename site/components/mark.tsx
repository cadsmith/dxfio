type Props = { size?: number }

export function Mark({ size = 28 }: Props) {
  return (
    <svg viewBox="0 0 24 24" width={size} height={size} aria-hidden="true">
      <defs>
        <linearGradient id="tile-gold" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0" stopColor="#F6C25A" />
          <stop offset="1" stopColor="#E2A230" />
        </linearGradient>
      </defs>
      <rect width="24" height="24" rx="5.4" fill={size >= 48 ? 'url(#tile-gold)' : '#F2B53A'} />
      <g transform="translate(3.6 3.6) scale(0.7)">
        <g fill="none" stroke="#1A1C21" strokeWidth={size >= 48 ? 2 : 2.7} strokeLinecap="round">
          <path d="M5 9 V20" />
          <circle cx="14.75" cy="14.5" r="5.5" />
        </g>
        {size >= 48 ? (
          <rect x="2.75" y="1.25" width="4.5" height="4.5" rx="0.9" fill="#FFFFFF" stroke="#1A1C21" strokeWidth="0.9" />
        ) : (
          <rect x="2.2" y="0.7" width="5.6" height="5.6" rx="1.12" fill="#1A1C21" />
        )}
      </g>
    </svg>
  )
}
